#pragma once
#include <vector>
#include <list>
#include "QuickMaths.h"


class Collider;
class RigidBody;
class Cell;

struct MTV
{
	Vec2 Normal;
	float Penetration;
};

struct Manifold
{
	RigidBody* rb1;
	RigidBody* rb2;
	Vec2 ContactPoint;
	Vec2 Normal;
	float Penetration;
};

struct CircleCollider
{
	Vec2 CenterLocal;
	Vec2 CenterWorld;
	glm::mat4 Transform;
	float Radius;
	RigidBody* Body = nullptr;
	bool IsSensor = false;
	Cell* Cell = nullptr;
	std::function<void(CircleCollider*)> CollisionCallback;
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
	void updateRigidbodies();

private:
	std::vector<CircleCollider*> mColliders;
	std::vector<RigidBody*> mRigidBodies;
	std::vector<Manifold> mManifolds;
	float mStep = 1/100.0f;
};

struct RigidBody
{
	Vec2 Position;
	Vec2 CenterOfMassLocal = Vec2(0, 0);
	Vec2 CenterOfMassWorld = Vec2(0, 0);
	Vec2 Correction = Vec2(0, 0);
	float InvInertia = 0.01f;
	float InvMass = 1;
	float AngularVelocity = 0;
	float Rotation = 0;
	Vec2 Acceleration;
	std::list<Vec2> Forces;
	std::list<Vec2> Impulses;
	Vec2 Velocity;

	void correctPosition(float delta);
	void onCollision(CircleCollider* other);
	float getInvMass();
	void setInvMass(float Value);
	Vec2 getAcceleration();
	void setAcceleration(Vec2);
	Vec2 getVelocity();
	void setVelocity(Vec2);
	void addForce(Vec2);
	void addImpulse(Vec2);
	std::list<Vec2>& getForces();
	std::list<Vec2>& getImpulses();
	void addCorrection(const Vec2& correction);

private:
	Vec2 mCorrection;
};