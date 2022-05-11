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
	mColliders.push_back(collider);
}

void PhysicsManager::add(RigidBody* rb)
{
	mRigidBodies.push_back(rb);
}

void PhysicsManager::remove(CircleCollider* collider)
{
	mColliders.erase(std::find(mColliders.begin(), mColliders.end(), collider));
	delete collider;
}

void PhysicsManager::remove(RigidBody* rb)
{
	mRigidBodies.erase(std::find(mRigidBodies.begin(), mRigidBodies.end(), rb));
	delete rb;
}

void PhysicsManager::update()
{
	static float accumulator = 0;
	accumulator += Time::DeltaSeconds;

	while (accumulator > mStep)
	{
		fixedUpdate();
		accumulator -= mStep;
	}
}

void PhysicsManager::fixedUpdate()
{
	updateRigidbodies();
	findCollisions();
	resolveCollisions();
	//GameObjectManager::getInstance().physicsUpdate(mStep);
}

void PhysicsManager::findCollisions()
{
	if (mColliders.size() < 2)
		return;

	for (int i = 0; i < mColliders.size() - 1; ++i)
	{
		for (int a = i + 1; a < mColliders.size(); ++a)
		{
			auto col1 = mColliders[i];
			auto col2 = mColliders[a];
			auto rb1 = col1->Body;
			auto rb2 = col2->Body;

			if (rb1 == rb2)
				continue;

			float sum_radii = col1->Radius + col2->Radius;
			Vec2 dir = col2->CenterWorld - col1->CenterWorld;

			if (dir.magnitude_squared() <= sum_radii * sum_radii)
			{
				if (col1->CollisionCallback)
					col1->CollisionCallback(col2);
				if (col2->CollisionCallback)
					col2->CollisionCallback(col1);

				if (col1->IsSensor || col2->IsSensor)
					continue;

				Manifold manifold;
				manifold.Penetration = (sum_radii - dir.magnitude()) * 0.5f;
				manifold.Normal = dir.normalize();
				manifold.ContactPoint = col1->CenterWorld + manifold.Normal * (col1->Radius - manifold.Penetration);
				manifold.rb1 = rb1;
				manifold.rb2 = rb2;
				mManifolds.push_back(manifold);
			}
		}
	}
}

void PhysicsManager::resolveCollisions()
{
	for (auto& man : mManifolds)
	{
		if (man.rb1->getInvMass() == 0 && man.rb2->getInvMass() == 0)
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
		auto contact_vec_1 = man.ContactPoint - man.rb1->CenterOfMassWorld;
		auto contact_vec_2 = man.ContactPoint - man.rb2->CenterOfMassWorld;
		auto angular_1 = contact_vec_1.cross(man.Normal);
		angular_1 *= angular_1;
		auto angular_2 = contact_vec_2.cross(man.Normal);
		angular_2 *= angular_2;
		j /= man.rb1->getInvMass() + man.rb2->getInvMass() + angular_1 + angular_2;
		Vec2 impulse = j * man.Normal;

		man.rb1->setVelocity(man.rb1->getVelocity() + Vec2(man.rb1->getInvMass() * impulse.X, man.rb1->getInvMass() * impulse.X));
		man.rb2->setVelocity(man.rb2->getVelocity() - Vec2(man.rb2->getInvMass() * impulse.X, man.rb2->getInvMass() * impulse.Y));
		man.rb1->AngularVelocity += man.rb1->InvInertia * contact_vec_1.cross(impulse);
		man.rb2->AngularVelocity -= man.rb2->InvInertia * contact_vec_2.cross(impulse);

		const float percent = 0.2f;
		const float slop = 0.3f;
		Vec2 correction = std::max(man.Penetration - slop, 0.0f) / (man.rb1->getInvMass() + man.rb2->getInvMass()) * percent * man.Normal;
		man.rb1->Correction += -man.rb1->getInvMass() * correction;
		man.rb2->Correction += man.rb2->getInvMass() * correction;
		man.Penetration -= correction.magnitude() * 2;
	}

	mManifolds.clear();
}

void PhysicsManager::updateRigidbodies()
{
	for (auto& body : mRigidBodies)
	{
		Vec2 force(0, 0);

		for (const auto& f : body->getForces())
			force += f;

		for (const auto& i : body->getImpulses())
			force += i;

		body->getImpulses().clear();
		body->setAcceleration(force * body->getInvMass());
		body->setVelocity(body->getVelocity() + body->getAcceleration() * mStep);
		body->Position += body->getVelocity() * mStep + body->Correction;
		body->Correction = Vec2(0, 0);
		body->CenterOfMassWorld = body->Position + body->CenterOfMassLocal;
		body->Rotation += body->AngularVelocity * mStep;
	}

	for (auto coll : mColliders)
	{
		auto tf =
			glm::translate(glm::mat4(1.0f), { coll->Body->Position.X, coll->Body->Position.Y, 0 }) *
			glm::rotate(glm::mat4(1.0f), coll->Body->Rotation, { 0, 0, 1 }) *
			glm::translate(glm::mat4(1.0f), { coll->CenterLocal.X, coll->CenterLocal.Y, 0 }) *
			glm::scale(glm::mat4(1.0f), { coll->Radius * 2, coll->Radius * 2, 1 });

		coll->Transform = tf;
		coll->CenterWorld = glm::vec2(tf * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
}

void RigidBody::correctPosition(float delta)
{
	//mGo->move(mVelocity * delta.asSeconds() + mCorrection);
	mCorrection.X = mCorrection.Y = 0;
}

void RigidBody::onCollision(CircleCollider* other)
{
}

float RigidBody::getInvMass()
{
	return InvMass;
}

void RigidBody::setInvMass(float Value)
{
	InvMass = Value;
}

Vec2 RigidBody::getAcceleration()
{
	return Acceleration;
}

void RigidBody::setAcceleration(Vec2 acc)
{
	Acceleration = acc;
}

Vec2 RigidBody::getVelocity()
{
	return Velocity;
}

void RigidBody::setVelocity(Vec2 vel)
{
	Velocity = vel;
}

void RigidBody::addForce(Vec2 force)
{
	Forces.push_back(force);
}

void RigidBody::addImpulse(Vec2 impulse)
{
	Impulses.push_back(impulse);
}

std::list<Vec2>& RigidBody::getForces()
{
	return Forces;
}

std::list<Vec2>& RigidBody::getImpulses()
{
	return Impulses;
}

void RigidBody::addCorrection(const Vec2& correction)
{
	mCorrection += correction;
}