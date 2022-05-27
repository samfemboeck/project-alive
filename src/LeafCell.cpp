#include "pch.h"
#include "LeafCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"

LeafCell::LeafCell(Organism* org, CircleCollider* collider) : 
	Cell(org, collider, "cell_leaf")
{
}

void LeafCell::tick()
{
	if (!isLit_)
		return;

	organism_->energy_ += Time::DeltaSeconds * 10.0f;
}

void LeafCell::onCollision(Cell* other)
{
}

void LeafCell::setLit(bool isLit)
{
	isLit_ = isLit;
}
