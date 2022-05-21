#include "pch.h"
#include "MoverCell.h"
#include "Organism.h"
#include "Engine/Physics.h"

MoverCell::MoverCell(Organism* org, CircleCollider* collider) :
	Cell(org, collider, "cell_mover")
{
}

void MoverCell::tick()
{
	Vec2f forward = b2Rot(organism_->rigidBody_->rotation).GetYAxis();
	organism_->rigidBody_->forces.push_back(forward);
}
