#pragma once
#include "Collider.h"

struct CircleCollider : Collider
{
	glm::vec2 Pos;
	float Radius;

	CircleCollider(RigidBody* rb, float radius, glm::vec2 offset = glm::vec2(0));
	static bool vs_circle(CircleCollider& a, CircleCollider& b, MTV& mtv);
	bool collides_with(Collider* collider, MTV& mtv) override;
	void update() override;
	void draw() override;
};
