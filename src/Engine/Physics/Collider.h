#pragma once

struct MTV;
class RigidBody;

class Collider
{
public:
	bool pm_is_trigger = false;
	RigidBody* pm_rigid_body = nullptr;
public:
	Collider(RigidBody* rb, glm::vec2 offset = glm::vec2(0));
	~Collider();
	virtual bool collides_with(Collider* collider, MTV& mtv) = 0;
	virtual void draw() = 0;
	virtual void update() = 0;
protected:
	glm::vec2 m_offset;
};

