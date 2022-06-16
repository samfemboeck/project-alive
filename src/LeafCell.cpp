#include "pch.h"
#include "LeafCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "OrganismManager.h"
#include "CorpseCell.h"

LeafCell::LeafCell() : 
	Cell("cell_leaf")
{
	type_ = Type::Plant;
}

void LeafCell::tick()
{
	if (organism_->wantsToDie())
		isLifetimeExpired_ = true;

	elapsed_ += Time::DeltaSeconds;

	if (elapsed_ >= 1)
	{
		organism_->setEnergy(organism_->getEnergy() + 1.0f);
		elapsed_ = 0.0f;
	}
}

float LeafCell::getNutritionValue() const
{
	return 1.0f;
}

CorpseCell* LeafCell::createCorpse() const
{
	return isLifetimeExpired_ ?  new CorpseCell(1.0f, localPos_) : nullptr ;
}

float LeafCell::getMass() const
{
	return 0.000001f;
}
