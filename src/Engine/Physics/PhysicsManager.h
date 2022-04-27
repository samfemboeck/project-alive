#pragma once

#include <vector>

class Collider;
class RigidBody;

struct MTV
{
	glm::vec2 normal;
	float penetration;
};

struct Manifold
{
	RigidBody* rb1;
	RigidBody* rb2;
	MTV mtv;
};

class SPhysics : System
{
public:
	static void add(Collider* collider);
	static void add(RigidBody* rb);
	static void remove(Collider* collider);
	static void remove(RigidBody* collider);
	static void update(float delta);
	static void debug_draw();
	static void fixed_update();
	static void find_collisions();
	static void resolve_collisions();
	static void update_rigid_bodies();
};
