#include "pch.h"
#include "MoverCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "OrganismManager.h"
#include "CorpseCell.h"

MoverCell::MoverCell() :
	Cell("cell_mover"),
	target_(Vec2f(0.0f, 0.0f))
{
}

void MoverCell::init()
{
	findNewTarget();
}

void MoverCell::tick()
{
	auto* rigidBody = organism_->getRigidBody();
	Vec2f forward = b2Rot(rigidBody->getRotation()).GetYAxis();
	rigidBody->addImpulse(forward * 5000.0f);

	if (rigidBody->getVelocity().magnitude() > speedMove_)
		rigidBody->setVelocity(rigidBody->getVelocity().normalized() * speedMove_);

	Vec2f vec1 = b2Rot(rigidBody->getRotation()).GetXAxis();
	Vec2f vec2 = (target_ - rigidBody->getPosition()).normalized();
	float dot = -b2Dot(vec1, vec2);
	rigidBody->addTorque(Random::floatRange(-50.0f, 50.0f));

	float distance = (target_ - organism_->getAABB()->bounds.center()).magnitude();
	if (distance <= 100.0f)
		findNewTarget();
}

CorpseCell* MoverCell::createCorpse() const
{
	return new CorpseCell(34.0f, localPos_);
}

void MoverCell::findNewTarget()
{
	float radius = Random::floatRange(500.0f, 1500.0f);
	auto min = organism_->getRigidBody()->getPosition() - radius;
	auto max = organism_->getRigidBody()->getPosition() + radius;
	target_ = Random::vec2Range(min, max);
}
