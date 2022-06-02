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
}

void LeafCell::tick()
{
	elapsed_ += Time::DeltaSeconds;
	if (elapsed_ >= 2)
	{
		organism_->setReproductionUrge(0);
		elapsed_ = 0.0f;
	}
}

float LeafCell::getNutritionValue() const
{
	return 34.0f;
}

CorpseCell* LeafCell::createCorpse() const
{
	return isLifetimeExpired_ ? nullptr : new CorpseCell(getNutritionValue(), localPos_);
}