#include "../../pch.h"
#include "Collider.h"
#include "PhysicsManager.h"
#include "RigidBody.h"

Collider::Collider(RigidBody* rb, glm::vec2 offset) :
	RB(rb),
	Offset(offset)
{
	PhysicsManager::add(this);
}

Collider::~Collider()
{
	PhysicsManager::remove(this);
}
