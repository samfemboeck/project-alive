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
		organism_->addEnergy(leafCell->getNutritionValue());
		org->removeCell(leafCell);
	}
	else
	{
		CorpseCell* corpseCell = dynamic_cast<CorpseCell*>(other);
		if (corpseCell)
		{
			organism_->addEnergy(corpseCell->getNutritionValue());
			org->removeCell(corpseCell);
		}
	}
}

CorpseCell* MouthCell::createCorpse() const
{
	return new CorpseCell(0.5f, localPos_);
}

void MouthCell::init()
{
}
