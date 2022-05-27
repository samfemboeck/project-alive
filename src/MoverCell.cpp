#include "pch.h"
#include "MoverCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"

MoverCell::MoverCell(Organism* org, CircleCollider* collider) :
	Cell(org, collider, "cell_mover")
{
}

void MoverCell::init()
{
	findNewTarget();
}

void MoverCell::tick()
{
	organism_->energy_ += Time::DeltaSeconds * 100.0f;

	Vec2f forward = b2Rot(organism_->rigidBody_->rotation).GetYAxis();
	organism_->rigidBody_->impulses.push_back(forward * 5000.0f);

	if (organism_->rigidBody_->velocity.magnitude() > speedMove_)
		organism_->rigidBody_->velocity = organism_->rigidBody_->velocity.normalized() * speedMove_;

	Vec2f vec1 = b2Rot(organism_->rigidBody_->rotation).GetXAxis();
	Vec2f vec2 = (target_ - organism_->rigidBody_->position).normalized();
	float dot = -b2Dot(vec1, vec2);
	organism_->rigidBody_->accelerationAngular = dot * speedTurn_;

	float distance = (target_ - organism_->aabb_->bounds.center()).magnitude();
	if (distance <= 100.0f)
		findNewTarget();
}

void MoverCell::findNewTarget()
{
	float radius = Random::floatRange(100.0f, 500.0f);
	auto min = organism_->rigidBody_->position - radius;
	auto max = organism_->rigidBody_->position + radius;
	target_ = Random::vec2Range(min, max);
}
