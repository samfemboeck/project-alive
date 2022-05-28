#include "../pch.h"
#include "Physics.h"
#include "Time.h"
#include "Timer.h"
#include "../Organism.h"

PhysicsManager& PhysicsManager::getInstance()
{
	static PhysicsManager instance;
	return instance;
}

void PhysicsManager::add(AABB* aabb)
{
	update(aabb);
	map(aabb);
	aabbs_.push_back(aabb);
}

void PhysicsManager::update()
{
	static float accumulator = 0;
	accumulator += Time::DeltaSeconds;


	while (accumulator > step_)
	{
		fixedUpdate();
		accumulator -= step_;
	}
}

void PhysicsManager::fixedUpdate()
{
	spatialHash_.clear();

	for (auto it = aabbs_.begin(); it != aabbs_.end();)
	{
		if ((*it)->wantsToBeDeleted)
		{
			delete *it;
			it = aabbs_.erase(it);
		}
		else
			it++;
	}

	for (auto* aabb : aabbs_)
	{
		update(aabb);
		map(aabb);
	}

	for (unsigned x = 0; x < EntityGrid::GridWidth; x++)
	{
		for (unsigned y = 0; y < EntityGrid::GridHeight; y++)
		{
			auto& entitiesInSquare = spatialHash_.get(x, y);

			for (unsigned i = 0; entitiesInSquare[i] && i < EntityGrid::MaxEntitiesPerSquare - 1; i++)
			{
				for (unsigned a = i + 1; a < EntityGrid::MaxEntitiesPerSquare && entitiesInSquare[a]; a++)
				{
					auto& collidersI = entitiesInSquare[i]->colliders;
					auto& collidersA = entitiesInSquare[a]->colliders;

					for (auto* colliderI : collidersI)
					{
						for (auto* colliderA : collidersA)
						{
							testCollision(colliderI, colliderA);
						}
					}
				}
			}
		}
	}

	resolveCollisions();
}

void PhysicsManager::testCollision(CircleCollider* col1, CircleCollider* col2)
{
	auto rb1 = col1->rigidBody;
	auto rb2 = col2->rigidBody;

	Vec2f dir = col2->centerWorld - col1->centerWorld;

	if (dir.x == 0.0f && dir.y == 0.0f)
		dir = col2->centerWorld - (col2->centerWorld - 1.0f);

	float sumRadii = col1->radius + col2->radius;

	if (dir.magnitude_squared() < sumRadii * sumRadii)
	{
		if (col1->collisionCallback)
			col1->collisionCallback(col2->cell);
		if (col2->collisionCallback)
			col2->collisionCallback(col1->cell);

		if (col1->isSensor || col2->isSensor)
			return;

		ManifoldResolve manifold;
		manifold.Penetration = (sumRadii - dir.magnitude()) * 0.5f;
		manifold.Normal = dir.normalized();
		manifold.ContactPoint = col1->centerWorld + manifold.Normal * (col1->radius - manifold.Penetration);
		manifold.rb1 = rb1;
		manifold.rb2 = rb2;
		manifolds_.push_back(manifold);
	}
}

bool PhysicsManager::circleVsCircle(Vec2f centerA, float radiusA, Vec2f centerB, float radiusB)
{
	float sumRadii = radiusA + radiusB;
	Vec2f dir = centerB - centerA;
	return dir.magnitude_squared() < sumRadii * sumRadii;
}

void PhysicsManager::resolveCollisions()
{
	for (auto& man : manifolds_)
	{
		if (man.rb1->invMass == 0 && man.rb2->invMass == 0)
		{
			continue;
		}

		Vec2f rv = man.rb2->getVelocity() - man.rb1->getVelocity();
		float velAlongNormal = rv.dot(man.Normal);

		if (velAlongNormal > 0)
		{
			continue;
		}

		float e = 0.1f;

		float j = (1 + e) * velAlongNormal;
		auto contact_vec_1 = man.ContactPoint - man.rb1->centerOfMassWorld;
		auto contact_vec_2 = man.ContactPoint - man.rb2->centerOfMassWorld;
		auto angular_1 = contact_vec_1.cross(man.Normal);
		angular_1 *= angular_1;
		auto angular_2 = contact_vec_2.cross(man.Normal);
		angular_2 *= angular_2;
		j /= man.rb1->invMass + man.rb2->invMass + angular_1 + angular_2;
		Vec2f impulse = j * man.Normal;

		man.rb1->setVelocity(man.rb1->getVelocity() + Vec2f(man.rb1->invMass * impulse.x, man.rb1->invMass * impulse.x));
		man.rb2->setVelocity(man.rb2->getVelocity() - Vec2f(man.rb2->invMass * impulse.x, man.rb2->invMass * impulse.y));
		man.rb1->velocityAngular += man.rb1->invInertia * contact_vec_1.cross(impulse);
		man.rb2->velocityAngular -= man.rb2->invInertia * contact_vec_2.cross(impulse);

		const float percent = 0.75f;
		const float slop = 0.0f;
		Vec2f correction = std::max(man.Penetration - slop, 0.0f) / (man.rb1->invMass + man.rb2->invMass) * percent * man.Normal;
		man.rb1->correction += -man.rb1->invMass * correction;
		man.rb2->correction += man.rb2->invMass * correction;
		man.Penetration -= correction.magnitude() * 2;
	}

	manifolds_.clear();
}

void PhysicsManager::squareCast(Vec2f start, Vec2f end, std::vector<AABB*>& out, RigidBody* ignore)
{
	Vec2i startGrid = spatialHash_.getLocalCoord(start);
	Vec2i endGrid = spatialHash_.getLocalCoord(end);

	if (startGrid.x < 0 || startGrid.y < 0 || endGrid.x >= EntityGrid::GridWidth || endGrid.y > EntityGrid::GridHeight)
		return;

	for (unsigned x = startGrid.x; x <= endGrid.x; x++)
	{
		for (unsigned y = startGrid.y; y <= endGrid.y; y++)
		{
			auto& entitiesInSquare = spatialHash_.get(x, y);
			for (auto* entity : entitiesInSquare)
			{
				if (!entity)
					break;

				if (entity->wantsToBeDeleted || entity->rigidBody == ignore)
					continue;

				out.push_back(entity);
			}
		}
	}
}

void PhysicsManager::draw()
{
	//spatialHash_.draw();
}

static std::random_device rd; // obtain a random number from hardware
static std::mt19937 gen(rd()); // seed the generator

bool PhysicsManager::findSpawnPosition(AABB* aabb, unsigned maxNearbyEntities, Vec2f& outPos)
{
	Vec2i gridMin = spatialHash_.getLocalCoord(aabb->bounds.min);
	Vec2i gridMax = spatialHash_.getLocalCoord(aabb->bounds.max);

	for (int x = gridMin.x - 1; x <= gridMax.x + 1; x++)
	{
		for (int y = gridMin.y - 1; y <= gridMax.y + 1; y++)
		{
			if ((x < gridMin.x || x > gridMax.x || y < gridMin.y || y > gridMax.y) && x >= 0 && x < EntityGrid::GridWidth && y >= 0 && y < EntityGrid::GridHeight)
			{
				unsigned numEntities = 0;
				auto& entitiesInSquare = spatialHash_.get(x, y);

				for (auto* entity : entitiesInSquare)
				{
					if (!entity)
						break;

					if (entity->wantsToBeDeleted)
						continue;

					numEntities++;
				}

				if (numEntities <= maxNearbyEntities)
				{
					float range = EntityGrid::SquareSize * 0.5f;
					outPos = spatialHash_.getWorldPos({ x, y }) + Random::floatRange(-range, range);
					return true;
				}
			}
		}
	}

	return false;
}

void PhysicsManager::update(AABB* aabb)
{
	auto* rigidBody = aabb->rigidBody;
	Vec2f force(0, 0);

	for (const auto& f : rigidBody->getForces())
		force += f;

	for (const auto& i : rigidBody->getImpulses())
		force += i;

	rigidBody->getImpulses().clear();
	rigidBody->acceleration = force * rigidBody->invMass;
	rigidBody->setVelocity(rigidBody->getVelocity() + rigidBody->acceleration * step_);
	rigidBody->position += rigidBody->getVelocity() * step_ + rigidBody->correction;
	rigidBody->setVelocity(rigidBody->getVelocity() * rigidBody->friction);
	rigidBody->correction = Vec2f(0, 0);
	rigidBody->centerOfMassWorld = rigidBody->position + rigidBody->centerOfMassLocal;
	rigidBody->velocityAngular += rigidBody->accelerationAngular * step_;
	rigidBody->rotation += rigidBody->velocityAngular * step_;
	rigidBody->velocityAngular *= rigidBody->dampingAngular;

	for (auto* coll : aabb->colliders)
	{
		auto tf =
			TRANSLATE(coll->rigidBody->position) *
			ROTATE(coll->rigidBody->rotation) *
			TRANSLATE(coll->centerLocal) *
			SCALE(coll->radius * 2);

		coll->transform = tf;
		coll->centerWorld = glm::vec2(tf * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	static constexpr float fMax = std::numeric_limits<float>::max();
	static constexpr float fMin = std::numeric_limits<float>::lowest();

	Vec2f min = { fMax, fMax };
	Vec2f max = { fMin, fMin };
	auto& colliders = aabb->colliders;

	for (const auto* collider : colliders)
	{
		const Vec2f& pos = collider->centerWorld;

		if (pos.x < min.x)
			min.x = pos.x;
		if (pos.y < min.y)
			min.y = pos.y;
		if (pos.x > max.x)
			max.x = pos.x;
		if (pos.y > max.y)
			max.y = pos.y;
	}

	min.x -= Cell::Size * 0.5f;
	min.y -= Cell::Size * 0.5f;
	max.x += Cell::Size * 0.5f;
	max.y += Cell::Size * 0.5f;

	aabb->bounds.min = min;
	aabb->bounds.max = max;
}

void PhysicsManager::map(AABB* aabb)
{
	spatialHash_.add(aabb);
}

bool PhysicsManager::hasValidPos(AABB* aabb)
{
	auto min = aabb->bounds.min;
	auto max = aabb->bounds.max;
	auto pos = spatialHash_.getPos();
	return min.x > pos.x && min.y > pos.y && max.x < pos.x + EntityGrid::GridWidth * EntityGrid::SquareSize && max.y < pos.y + EntityGrid::GridHeight * EntityGrid::SquareSize;
}

PhysicsManager::PhysicsManager()
{

}

void RigidBody::onCollision(CircleCollider* other)
{
}

Vec2f RigidBody::getVelocity()
{
	return velocity;
}

void RigidBody::setVelocity(Vec2f vel)
{
	velocity = vel;
}

void RigidBody::addForce(Vec2f force)
{
	forces.push_back(force);
}

void RigidBody::addImpulse(Vec2f impulse)
{
	impulses.push_back(impulse);
}

std::list<Vec2f>& RigidBody::getForces()
{
	return forces;
}

std::list<Vec2f>& RigidBody::getImpulses()
{
	return impulses;
}

CircleCollider::~CircleCollider()
{
}
