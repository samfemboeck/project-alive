#include "pch.h"
#include "MoverCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "OrganismManager.h"

MoverCell::MoverCell(Organism* org, CircleCollider* collider) :
	Cell(org, collider, "cell_mover"),
	ttl_(Random::longRange(8000, 9000))
{
}

void MoverCell::init()
{
	findNewTarget();
}

void MoverCell::tick()
{
	if (organism_->getAgeMs() >= ttl_)
		organism_->markForDeath();

	auto* rigidBody = organism_->getRigidBody();
	Vec2f forward = b2Rot(rigidBody->rotation).GetYAxis();
	rigidBody->impulses.push_back(forward * 5000.0f);

	if (rigidBody->velocity.magnitude() > speedMove_)
		rigidBody->velocity = rigidBody->velocity.normalized() * speedMove_;

	Vec2f vec1 = b2Rot(rigidBody->rotation).GetXAxis();
	Vec2f vec2 = (target_ - rigidBody->position).normalized();
	float dot = -b2Dot(vec1, vec2);
	rigidBody->accelerationAngular = dot * speedTurn_;

	float distance = (target_ - organism_->getAABB()->bounds.center()).magnitude();
	if (distance <= 100.0f)
		findNewTarget();
}

void MoverCell::onCollision(Cell* other)
{	
	if (other->organism_->isLeaf())
	{
		other->organism_->markForDeletion();
		energy_ += 34.0f;
	}
	else if (other->organism_->isCorpse())
	{
		other->organism_->markForDeletion();
		energy_ += 50.0f;
	}

	if (energy_ >= 100.0f)
	{
		organism_->setReproductionCount(1);
		energy_ = 0.0f;
	}
}

void MoverCell::findNewTarget()
{
	float radius = Random::floatRange(500.0f, 1000.0f);
	auto min = organism_->getRigidBody()->position - radius;
	auto max = organism_->getRigidBody()->position + radius;
	target_ = Random::vec2Range(min, max);
}
