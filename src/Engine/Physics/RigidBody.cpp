#include "../../pch.h"
#include "RigidBody.h"
#include "PhysicsManager.h"

RigidBody::RigidBody()
{
	log("rb pos: {0} {1}", Transform.Position.x, Transform.Position.y);
	PhysicsManager::add(this);
}

RigidBody::~RigidBody()
{
	PhysicsManager::remove(this);
}

void RigidBody::correct_position(float delta)
{
	Transform.Position += Velocity * delta + Correction;
	Correction.x = Correction.y = 0;
}

void RigidBody::on_collision(Collider* other)
{
}

void RigidBody::set_ignore(std::string layer, bool ignore)
{
	IgnoredLayers[layer] = ignore;
}

void RigidBody::set_ignore(RigidBody* rb, bool ignore)
{
	IgnoredRBs[rb] = ignore;
}

bool RigidBody::has_ignore(RigidBody* rb)
{
	auto it = IgnoredRBs.find(rb);
	if (it != IgnoredRBs.end() && IgnoredRBs[rb])
		return true;

	auto it2 = IgnoredLayers.find(rb->Layer);
	if (it2 != IgnoredLayers.end() && IgnoredLayers[rb->Layer])
		return true;

	return false;
}

void RigidBody::add_correction(const glm::vec2& correction)
{
	Correction += correction;
}

void RigidBodySys::update()
{
	for (auto rb : m_components_active)
	{
		rb->Transform.Position += rb->Velocity;
	}
}
