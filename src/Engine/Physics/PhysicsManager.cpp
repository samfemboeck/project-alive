#include "../../pch.h"
#include "PhysicsManager.h"
#include "Collider.h"
#include "RigidBody.h"

static std::vector<Collider*> s_colliders;
static std::vector<RigidBody*> s_rigid_bodies;
static std::vector<Manifold> s_manifolds;
static float s_time_step = 1 / 100.0f;


void PhysicsManager::add(Collider* collider)
{
	s_colliders.push_back(collider);
}

void PhysicsManager::add(RigidBody* rb)
{
	s_rigid_bodies.push_back(rb);
}

void PhysicsManager::remove(Collider* collider)
{
	s_colliders.erase(std::find(s_colliders.begin(), s_colliders.end(), collider));
}

void PhysicsManager::remove(RigidBody* rb)
{
	s_rigid_bodies.erase(std::find(s_rigid_bodies.begin(), s_rigid_bodies.end(), rb));
}

void PhysicsManager::update(float delta)
{
	static float accumulator = 0;
	accumulator += delta;

	for (Collider* collider : s_colliders)
	{
		collider->update();
	}

	while (accumulator > s_time_step)
	{
		fixed_update();
		accumulator -= s_time_step;
	}
}

void PhysicsManager::debug_draw()
{
	for (Collider* collider : s_colliders)
	{
		collider->draw();
	}
}

void PhysicsManager::fixed_update()
{
	find_collisions();
	resolve_collisions();
	//GameObjectManager::getInstance().physicsUpdate(mStep);
	update_rigid_bodies();
}

void PhysicsManager::find_collisions()
{
	if (s_colliders.size() < 2)
		return;

	for (int i = 0; i < s_colliders.size() - 1; ++i)
	{
		for (int a = i + 1; a < s_colliders.size(); ++a)
		{
			auto col1 = s_colliders[i];
			auto col2 = s_colliders[a];
			auto rb1 = col1->pm_rigid_body;
			auto rb2 = col2->pm_rigid_body;

			if (rb1 == rb2 ||
				rb1->has_ignore(rb2) ||
				rb2->has_ignore(rb1))
				continue;

			MTV mtv;

			if (col1->collides_with(col2, mtv))
			{
				rb1->on_collision(col2);
				rb2->on_collision(col1);

				if (col1->pm_is_trigger || col2->pm_is_trigger)
					continue;

				Manifold manifold;
				manifold.mtv = mtv;
				manifold.rb1 = rb1;
				manifold.rb2 = rb2;
				s_manifolds.push_back(manifold);
			}
		}
	}
}

void PhysicsManager::resolve_collisions()
{
	for (auto& man : s_manifolds)
	{
		if (man.rb1->pm_inv_mass == 0 && man.rb2->pm_inv_mass == 0)
			continue;

		glm::vec2 rv = man.rb2->pm_velocity - man.rb1->pm_velocity;
		float velAlongNormal = (rv.x * man.mtv.normal.x + rv.y * man.mtv.normal.y);

		if (velAlongNormal > 0)
			continue;

		float e = 0.1f;

		float j = (1 + e) * velAlongNormal;
		j /= man.rb1->pm_inv_mass + man.rb2->pm_inv_mass;
		glm::vec2 impulse = j * man.mtv.normal;

		man.rb1->pm_velocity = man.rb1->pm_velocity + glm::vec2(man.rb1->pm_inv_mass * impulse.x, man.rb1->pm_inv_mass * impulse.y);
		man.rb2->pm_velocity = man.rb2->pm_velocity - glm::vec2(man.rb2->pm_inv_mass * impulse.x, man.rb2->pm_inv_mass * impulse.y);

		const float percent = 0.5f;
		const float slop = 0.f;
		glm::vec2 correction = std::max(man.mtv.penetration - slop, 0.0f) / (man.rb1->pm_inv_mass + man.rb2->pm_inv_mass) * percent * man.mtv.normal;
		man.rb1->add_correction(-man.rb1->pm_inv_mass * correction);
		man.rb2->add_correction(man.rb2->pm_inv_mass * correction);
	}

	s_manifolds.clear();
}

void PhysicsManager::update_rigid_bodies()
{
	for (auto& body : s_rigid_bodies)
	{
		glm::vec2 forces = glm::vec2(0);

		for (const auto& f : body->pm_forces)
			forces += f;

		for (const auto& i : body->pm_impulses)
			forces += i;

		body->pm_impulses.clear();
		body->pm_acceleration = forces * body->pm_inv_mass;
		body->pm_velocity = body->pm_velocity + body->pm_acceleration * s_time_step;
		body->correct_position(s_time_step);
	}
}
