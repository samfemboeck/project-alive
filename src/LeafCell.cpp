#include "pch.h"
#include "LeafCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "OrganismManager.h"
#include "CorpseCell.h"

LeafCell::LeafCell() : 
	Cell("cell_leaf"),
	ttl_(Random::longRange(10000, 30000))
{
}

void LeafCell::tick()
{
	if (organism_->getAgeMs() >= ttl_)
	{
		isLifetimeExpired_ = true;
		organism_->markForDeath();
	}

	elapsed_ += Time::DeltaSeconds;
	if (elapsed_ >= 0.5f)
	{
		organism_->setReproductionUrge(1);
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

long LeafCell::getTtl()
{
	return ttl_;
}
