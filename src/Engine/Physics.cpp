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
	aabbs_.erase(std::find(aabbs_.begin(), aabbs_.end(), aabb));
	aabb->isDeleted = true;
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
	updateRigidBodies();

	unsigned collisionQueries = 0;

	for (unsigned x = 0; x < SpatialHash::GridWidth; x++)
	{
		for (unsigned y = 0; y < SpatialHash::GridHeight; y++)
		{
			auto& entitiesInSquare = spatialHash_.get(x, y);

			for (unsigned i = 0; entitiesInSquare[i] && i < SpatialHash::MaxEntitiesPerSquare - 1; i++)
			{
				for (unsigned a = i + 1; entitiesInSquare[a] && a < SpatialHash::MaxEntitiesPerSquare; a++)
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

	LOG("Total Queries: {}", collisionQueries);

	resolveCollisions();
}

void PhysicsManager::testCollision(CircleCollider* col1, CircleCollider* col2)
{
	auto rb1 = col1->rigidBody;
	auto rb2 = col2->rigidBody;

	float sum_radii = col1->radius + col2->radius;
	Vec2f dir = col2->centerWorld - col1->centerWorld;

	if (dir.magnitude_squared() <= sum_radii * sum_radii)
	{
		if (col1->collisionCallback)
			col1->collisionCallback(col2->cell);
		if (col2->collisionCallback)
			col2->collisionCallback(col1->cell);

		if (col1->isSensor || col2->isSensor)
			return;

		ManifoldResolve manifold;
		manifold.Penetration = (sum_radii - dir.magnitude()) * 0.5f;
		manifold.Normal = dir.normalize();
		manifold.ContactPoint = col1->centerWorld + manifold.Normal * (col1->radius - manifold.Penetration);
		manifold.rb1 = rb1;
		manifold.rb2 = rb2;
		manifolds_.push_back(manifold);
	}
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
	{
		auto toWorld = TRANSLATE(aabb->rigidBody->position);
		aabb->boundsWorld.min = glm::vec2(toWorld * glm::vec4(aabb->boundsLocal.min.x, aabb->boundsLocal.min.y, 0.0f, 1.0f));
		aabb->boundsWorld.max = glm::vec2(toWorld * glm::vec4(aabb->boundsLocal.max.x, aabb->boundsLocal.max.y, 0.0f, 1.0f));
		spatialHash_.add(aabb);
	}
}

void PhysicsManager::setSpatialHashPosition(Vec2f pos)
{
	spatialHash_.setPos(pos);
}

bool PhysicsManager::squareCast(Vec2f start, Vec2f end, std::vector<AABB*>& out)
{
	Vec2i startGrid = spatialHash_.getCoord(start);
	Vec2i endGrid = spatialHash_.getCoord(end);

	for (unsigned x = startGrid.x; x <= endGrid.x; x++)
	{
		for (unsigned y = startGrid.y; y <= endGrid.y; y++)
		{
			auto& entitiesInSquare = spatialHash_.get(x, y);
			for (auto* entity : entitiesInSquare)
			{
				if (!entity)
					break;

				if (entity->isDeleted)
					continue;

				out.push_back(entity);
			}
		}
	}

	return out.size() > 0;
}

void PhysicsManager::draw()
{
	spatialHash_.draw();
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
