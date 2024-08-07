#include "pch.h"
#include "LeafCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "OrganismManager.h"
#include "CorpseCell.h"

LeafCell::LeafCell() : 
	Cell("cell_plant.png")
{
	type_ = Type::Plant;
}

void LeafCell::tick()
{
	organism_->setEnergy(organism_->getEnergy() + organism_->getHunger() * Time::DeltaSeconds / 20.0f);
	/*
	if (organism_->getAge() >= organism_->getTTL() * (1.0f / 2.0f) && lifetimeFlag_ == 0)
	{
		lifetimeFlag_ = 1;
		organism_->setEnergy(organism_->getEnergy() + organism_->getHunger());
	}

	if (organism_->getAge() >= organism_->getTTL() - 100 && lifetimeFlag_ == 1)
	{
		lifetimeFlag_ = 2;
		organism_->setEnergy(organism_->getEnergy() + organism_->getHunger());
	}
	*/
}

float LeafCell::getNutritionValue() const
{
	return 1.0f;
}

CorpseCell* LeafCell::createCorpse() const
{
	return organism_->wantsToDie() ? new CorpseCell(1.0f, localPos_, true) : nullptr;
}

float LeafCell::getMass() const
{
	return 5.0f;
}

char LeafCell::getSymbol()
{
	return 'P';
}
