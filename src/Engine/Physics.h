#pragma once
#include <vector>
#include <list>
#include "QuickMaths.h"


class Collider;
class RigidBody;
class Cell;

struct ManifoldResolve
{
	RigidBody* rb1;
	RigidBody* rb2;
	Vec2 ContactPoint;
	Vec2 Normal;
	float Penetration;
};

struct CircleCollider
{
	Vec2 centerLocal;
	Vec2 centerWorld;
	glm::mat4 transform;
	float radius;
	RigidBody* body = nullptr;
	bool isSensor = false;
	Cell* cell = nullptr;
	std::function<void(Cell*)> collisionCallback;
};

class PhysicsManager
{
public:
	static PhysicsManager& getInstance();
	void add(CircleCollider* collider);
	void add(RigidBody* rb);
	void remove(CircleCollider* collider);
	void remove(RigidBody* collider);
	void update();
	void fixedUpdate();
	void findCollisions();
	void resolveCollisions();
	void updateRigidBodies();

private:
	std::vector<CircleCollider*> colliders_;
	std::vector<RigidBody*> rigidBodies_;
	std::vector<ManifoldResolve> manifolds_;
	float step_ = 1/100.0f;
};

struct RigidBody
{
	inline static float LinearFriction = 0.9f;

	float friction = LinearFriction;
	Vec2 position;
	Vec2 centerOfMassLocal = Vec2(0, 0);
	Vec2 centerOfMassWorld = Vec2(0, 0);
	Vec2 correction = Vec2(0, 0);
	float invInertia = 0.01f;
	float invMass = 1;
	float velocityAngular = 0;
	float rotation = 0;
	Vec2 acceleration;
	std::list<Vec2> forces;
	std::list<Vec2> impulses;
	Vec2 velocity;

	void onCollision(CircleCollider* other);
	Vec2 getVelocity();
	void setVelocity(Vec2);
	void addForce(Vec2);
	void addImpulse(Vec2);
	std::list<Vec2>& getForces();
	std::list<Vec2>& getImpulses();
};