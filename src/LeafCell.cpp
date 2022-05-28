#include "pch.h"
#include "LeafCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "OrganismManager.h"

LeafCell::LeafCell(Organism* org, CircleCollider* collider) : 
	Cell(org, collider, "cell_leaf"),
	ttl_(Random::longRange(18000, 19000))
{
}

void LeafCell::tick()
{
	if (organism_->getAgeMs() >= ttl_)
		organism_->markForDeletion();

	elapsed_ += Time::DeltaSeconds;
	if (elapsed_ >= 1.0f)
	{
		organism_->setReproductionCount(1);
		elapsed_ = 0.0f;
	}
}

void LeafCell::onCollision(Cell* other)
{
}