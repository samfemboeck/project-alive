#pragma once
#include <vector>
#include <list>
#include <functional>
#include "QuickMaths.h"
#include "EntityGrid.h"

struct RigidBody;
class Cell;

struct ManifoldResolve
{
	RigidBody* rb1;
	RigidBody* rb2;
	Vec2f ContactPoint;
	Vec2f Normal;
	float Penetration;
};


struct CircleCollider
{
	~CircleCollider();
	Vec2f centerLocal;
	Vec2f centerWorld;
	glm::mat4 transform;
	float radius;
	RigidBody* rigidBody = nullptr;
	bool isSensor = false;
	Cell* cell = nullptr;
	std::function<void(Cell*,Cell*)> collisionCallback;
};

struct Bounds
{
	Vec2f min;
	Vec2f max;

	Vec2f center()
	{
		return min + ((max - min) * 0.5f);
	}
};

struct AABB
{
	Bounds bounds;
	RigidBody* rigidBody;
	std::vector<CircleCollider*> colliders;
	bool isDeleted = false;
};

class PhysicsManager
{
public:
	static bool circleVsCircle(Vec2f centerA, float radiusA, Vec2f centerB, float radiusB);
	static PhysicsManager& getInstance();
	void add(AABB* aabb);
	void update();
	void fixedUpdate();
	void testCollision(CircleCollider* collA, CircleCollider* collB);
	void resolveCollisions();
	void squareCast(Vec2f start, Vec2f end, std::vector<AABB*>& out, RigidBody* ignore = nullptr);
	void draw();
	bool findSpawnPosition(AABB* aabb, unsigned maxNearbyEntities, Vec2f& outpos);
	void update(AABB* aabb);
	void map(AABB* aabb);
	bool hasValidPos(AABB* aabb);

private:
	PhysicsManager();
private:
	std::vector<AABB*> aabbs_;
	std::vector<ManifoldResolve> manifolds_;
	EntityGrid spatialHash_;
	float step_ = 1/100.0f;
};

struct RigidBody
{
	inline static float LinearFriction = 0.9f;

	float friction = LinearFriction;
	Vec2f position = Vec2f(0, 0);
	Vec2f centerOfMassLocal = Vec2f(0, 0);
	Vec2f centerOfMassWorld = Vec2f(0, 0);
	Vec2f correction = Vec2f(0, 0);
	float invInertia = 0.01f;
	float invMass = 1;
	float velocityAngular = 0;
	float accelerationAngular = 0;
	float dampingAngular = 0.99f;
	float rotation = 0;
	Vec2f acceleration;
	std::list<Vec2f> forces;
	std::list<Vec2f> impulses;
	Vec2f velocity;

	void onCollision(CircleCollider* other);
	Vec2f getVelocity();
	void setVelocity(Vec2f);
	void addForce(Vec2f);
	void addImpulse(Vec2f);
	std::list<Vec2f>& getForces();
	std::list<Vec2f>& getImpulses();
};