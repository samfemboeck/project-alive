#include "../../pch.h"
#include "RigidBody.h"
#include "PhysicsManager.h"

RigidBody::RigidBody()
{
	log("rb pos: {0} {1}", pm_transform.pm_position.x, pm_transform.pm_position.y); 
	PhysicsManager::add(this);
}

RigidBody::~RigidBody()
{
	PhysicsManager::remove(this);
}

void RigidBody::correct_position(float delta)
{
	pm_transform.pm_position += pm_velocity * delta + m_correction;
	m_correction.x = m_correction.y = 0;
}

void RigidBody::on_collision(Collider* other)
{
}

void RigidBody::set_ignore(std::string layer, bool ignore)
{
	m_ignored_layers[layer] = ignore;
}

void RigidBody::set_ignore(RigidBody* rb, bool ignore)
{
	m_ignored_rbs[rb] = ignore;
}

bool RigidBody::has_ignore(RigidBody* rb)
{
	auto it = m_ignored_rbs.find(rb);
	if (it != m_ignored_rbs.end() && m_ignored_rbs[rb])
		return true;

	auto it2 = m_ignored_layers.find(rb->pm_layer);
	if (it2 != m_ignored_layers.end() && m_ignored_layers[rb->pm_layer])
		return true;

	return false;
}

void RigidBody::add_correction(const glm::vec2& correction)
{
	m_correction += correction;
}
