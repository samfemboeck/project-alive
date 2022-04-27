#pragma once

struct MTV;
class RigidBody;

struct Collider
{
	bool IsTrigger = false;
	RigidBody* RB = nullptr;
	glm::vec2 Offset;

	Collider(RigidBody* rb, glm::vec2 offset = glm::vec2(0));
	~Collider();
	virtual bool collides_with(Collider* collider, MTV& mtv) = 0;
	virtual void draw() = 0;
	virtual void update() = 0;
};

