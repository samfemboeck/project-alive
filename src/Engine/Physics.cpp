#include "../pch.h"
#include "Physics.h"
#include "Time.h"

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
	findCollisions();
	resolveCollisions();
	//GameObjectManager::getInstance().physicsUpdate(mStep);
}

void PhysicsManager::findCollisions()
{
	if (colliders_.size() < 2)
		return;

	for (int i = 0; i < colliders_.size() - 1; ++i)
	{
		for (int a = i + 1; a < colliders_.size(); ++a)
		{
			auto col1 = colliders_[i];
			auto col2 = colliders_[a];
			auto rb1 = col1->body;
			auto rb2 = col2->body;

			if (rb1 == rb2)
				continue;

			float sum_radii = col1->radius + col2->radius;
			Vec2 dir = col2->centerWorld - col1->centerWorld;

			if (dir.magnitude_squared() <= sum_radii * sum_radii)
			{
				if (col1->collisionCallback)
					col1->collisionCallback(col2);
				if (col2->collisionCallback)
					col2->collisionCallback(col1);

				if (col1->isSensor || col2->isSensor)
					continue;

				ManifoldResolve manifold;
				manifold.Penetration = (sum_radii - dir.magnitude()) * 0.5f;
				manifold.Normal = dir.normalize();
				manifold.ContactPoint = col1->centerWorld + manifold.Normal * (col1->radius - manifold.Penetration);
				manifold.rb1 = rb1;
				manifold.rb2 = rb2;
				manifolds_.push_back(manifold);
			}
		}
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

		Vec2 rv = man.rb2->getVelocity() - man.rb1->getVelocity();
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
		Vec2 impulse = j * man.Normal;

		man.rb1->setVelocity(man.rb1->getVelocity() + Vec2(man.rb1->invMass * impulse.x, man.rb1->invMass * impulse.x));
		man.rb2->setVelocity(man.rb2->getVelocity() - Vec2(man.rb2->invMass * impulse.x, man.rb2->invMass * impulse.y));
		man.rb1->velocityAngular += man.rb1->invInertia * contact_vec_1.cross(impulse);
		man.rb2->velocityAngular -= man.rb2->invInertia * contact_vec_2.cross(impulse);

		const float percent = 0.75f;
		const float slop = 0.0f;
		Vec2 correction = std::max(man.Penetration - slop, 0.0f) / (man.rb1->invMass + man.rb2->invMass) * percent * man.Normal;
		man.rb1->correction += -man.rb1->invMass * correction;
		man.rb2->correction += man.rb2->invMass * correction;
		man.Penetration -= correction.magnitude() * 2;
	}

	manifolds_.clear();
}

void PhysicsManager::updateRigidBodies()
{
	for (auto& body : rigidBodies_)
	{
		Vec2 force(0, 0);

		for (const auto& f : body->getForces())
			force += f;

		for (const auto& i : body->getImpulses())
			force += i;

		body->getImpulses().clear();
		body->acceleration = force * body->invMass;
		body->setVelocity(body->getVelocity() + body->acceleration * step_);
		body->position += body->getVelocity() * step_ + body->correction;
		body->setVelocity(body->getVelocity() * body->friction);
		body->correction = Vec2(0, 0);
		body->centerOfMassWorld = body->position + body->centerOfMassLocal;
		body->rotation += body->velocityAngular * step_;
	}

	for (auto coll : colliders_)
	{
		auto tf =
			glm::translate(glm::mat4(1.0f), { coll->body->position.x, coll->body->position.y, 0 }) *
			glm::rotate(glm::mat4(1.0f), coll->body->rotation, { 0, 0, 1 }) *
			glm::translate(glm::mat4(1.0f), { coll->centerLocal.x, coll->centerLocal.y, 0 }) *
			glm::scale(glm::mat4(1.0f), { coll->radius * 2, coll->radius * 2, 1 });

		coll->transform = tf;
		coll->centerWorld = glm::vec2(tf * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
}

void RigidBody::onCollision(CircleCollider* other)
{
}
Vec2 RigidBody::getVelocity()
{
	return velocity;
}

void RigidBody::setVelocity(Vec2 vel)
{
	velocity = vel;
}

void RigidBody::addForce(Vec2 force)
{
	forces.push_back(force);
}

void RigidBody::addImpulse(Vec2 impulse)
{
	impulses.push_back(impulse);
}

std::list<Vec2>& RigidBody::getForces()
{
	return forces;
}

std::list<Vec2>& RigidBody::getImpulses()
{
	return impulses;
}
