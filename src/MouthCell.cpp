#include "MouthCell.h"
#include "CorpseCell.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "pch.h"

MouthCell::MouthCell() :
	Cell("cell_mouth")
{
}

void MouthCell::onCollision(Cell* other)
{
	if (other->wantsToBeDeleted())
		return;

	Organism* org = other->getOrganism();
	
	LeafCell* leafCell = dynamic_cast<LeafCell*>(other);
	if (leafCell)
	{
		energy_ += leafCell->getNutritionValue();
		org->removeCell(leafCell);
	}
	else
	{
		CorpseCell* corpseCell = dynamic_cast<CorpseCell*>(other);
		if (corpseCell)
		{
			energy_ += corpseCell->getNutritionValue();
			org->removeCell(corpseCell);
		}
	}


	if (energy_ >= hunger_)
	{
		organism_->setReproductionUrge(organism_->getReproductionUrge() + 1);
		energy_ = 0.0f;
	}
}

CorpseCell* MouthCell::createCorpse() const
{
	return new CorpseCell(34.0f, localPos_);
}

void MouthCell::init()
{
	hunger_ = organism_->getSize() * 34.0f + 34.0f;
}
