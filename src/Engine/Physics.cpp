#include "../pch.h"
#include "Physics.h"
#include "Time.h"
#include "Timer.h"

PhysicsManager& PhysicsManager::getInstance()
{
	static PhysicsManager instance;
	return instance;
}

void PhysicsManager::add(CircleCollider* collider)
{
	colliders_.push_back(collider);
}

void PhysicsManager::add(RigidBody* rb)
{
	rigidBodies_.push_back(rb);
}

void PhysicsManager::add(AABB* aabb)
{
	aabbs_.push_back(aabb);
	update(aabb);
}

void PhysicsManager::remove(CircleCollider* collider)
{
	colliders_.erase(std::find(colliders_.begin(), colliders_.end(), collider));
	delete collider;
}

void PhysicsManager::remove(RigidBody* rb)
{
	rigidBodies_.erase(std::find(rigidBodies_.begin(), rigidBodies_.end(), rb));
	delete rb;
}

void PhysicsManager::remove(AABB* aabb)
{
	aabb->isDeleted = true;
}

void PhysicsManager::update()
{
	static float accumulator = 0;
	accumulator += Time::DeltaSeconds;

	for (auto it = aabbs_.begin(); it != aabbs_.end();)
	{
		if ((*it)->isDeleted)
		{
			delete *it;
			it = aabbs_.erase(it);
		}
		else
			it++;
	}

	while (accumulator > step_)
	{
		fixedUpdate();
		accumulator -= step_;
	}
}

void PhysicsManager::fixedUpdate()
{
	updateRigidBodies();

	unsigned collisionQueries = 0;

	for (unsigned x = 0; x < EntityGrid::GridWidth; x++)
	{
		for (unsigned y = 0; y < EntityGrid::GridHeight; y++)
		{
			auto& entitiesInSquare = spatialHash_.get(x, y);

			for (unsigned i = 0; entitiesInSquare[i] && i < EntityGrid::MaxEntitiesPerSquare - 1; i++)
			{
				for (unsigned a = i + 1; a < EntityGrid::MaxEntitiesPerSquare && entitiesInSquare[a]; a++)
				{
					auto& collidersI = entitiesInSquare[i]->rigidBody->colliders;
					auto& collidersA = entitiesInSquare[a]->rigidBody->colliders;
					for (auto* colliderI : collidersI)
					{
						for (auto* colliderA : collidersA)
						{
							testCollision(colliderI, colliderA);
							collisionQueries++;
						}
					}
				}
			}
		}
	}

	//LOG("Total Queries: {}", collisionQueries);

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
		manifold.Normal = dir.normalize();
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

void PhysicsManager::updateRigidBodies()
{
	for (auto* rigidBody : rigidBodies_)
	{
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
		rigidBody->rotation += rigidBody->velocityAngular * step_;
	}

	for (auto* coll : colliders_)
	{
		auto tf =
			TRANSLATE(coll->rigidBody->position) *
			ROTATE(coll->rigidBody->rotation) *
			TRANSLATE(coll->centerLocal) *
			SCALE(coll->radius * 2);

		coll->transform = tf;
		coll->centerWorld = glm::vec2(tf * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	spatialHash_.clear();

	for (auto* aabb : aabbs_)
		if (!aabb->isDeleted)
			update(aabb);
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

				if (entity->isDeleted || entity->rigidBody == ignore)
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

bool PhysicsManager::findAdjacentPosition(AABB* aabb, unsigned maxNearbyEntities, Vec2f& outPos)
{
	int width = aabb->boundsLocal.max.x - aabb->boundsLocal.min.x;
	int height = aabb->boundsLocal.max.y - aabb->boundsLocal.min.y;

	std::array<Vec2i, 8> offsets =
	{
		Vec2i{-width, 0},
		{-width, height},
		{0, height},
		{width, height},
		{width, 0},
		{-width, -height},
		{0, -height},
	};

	for (const auto& offset : offsets)
	{
		Vec2f newStart = { aabb->boundsWorld.min.x + offset.x, aabb->boundsWorld.min.y + offset.y };
		Vec2f newEnd = { aabb->boundsWorld.max.x + offset.x, aabb->boundsWorld.max.y + offset.y };
		
		unsigned numEntities = 0;

		Vec2i newGridStart = spatialHash_.getLocalCoord(newStart);
		Vec2i newGridEnd = spatialHash_.getLocalCoord(newEnd);

		if (newGridStart.x < 0 || newGridStart.y < 0 || newGridEnd.x >= EntityGrid::GridWidth || newGridEnd.y > EntityGrid::GridHeight)
			return false;

		for (unsigned x = newGridStart.x; x <= newGridEnd.x; x++)
		{
			for (unsigned y = newGridStart.y; y <= newGridEnd.y; y++)
			{
				auto& entitiesInSquare = spatialHash_.get(x, y);
				for (auto* entity : entitiesInSquare)
				{
					if (!entity)
						break;

					if (entity->isDeleted)
						continue;

					numEntities++;
				}
			}
		}

		if (numEntities <= maxNearbyEntities)
		{
			outPos = newStart + ((newEnd - newStart) * 0.5f);
			return true;
		}
	}
	
	return false;
}

void PhysicsManager::update(AABB* aabb)
{
	auto toWorld = TRANSLATE(aabb->rigidBody->position);
	aabb->boundsWorld.min = glm::vec2(toWorld * glm::vec4(aabb->boundsLocal.min.x, aabb->boundsLocal.min.y, 0.0f, 1.0f));
	aabb->boundsWorld.max = glm::vec2(toWorld * glm::vec4(aabb->boundsLocal.max.x, aabb->boundsLocal.max.y, 0.0f, 1.0f));	

	spatialHash_.add(aabb);
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
