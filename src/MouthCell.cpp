#include "MouthCell.h"
#include "CorpseCell.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "pch.h"

MouthCell::MouthCell() :
	Cell("cell_mouth")
{
	type_ = Type::Mouth;
}

void MouthCell::onCollision(Cell* other)
{
	if (other->wantsToBeDeleted())
		return;

	LeafCell* leafCell = dynamic_cast<LeafCell*>(other);
	if (leafCell)
	{
		organism_->setEnergy(organism_->getEnergy() + leafCell->getNutritionValue());
		other->getOrganism()->removeCell(leafCell);
	}
	else
	{
		CorpseCell* corpseCell = dynamic_cast<CorpseCell*>(other);
		if (corpseCell)
		{
			organism_->setEnergy(organism_->getEnergy() + corpseCell->getNutritionValue());
			other->getOrganism()->removeCell(corpseCell);
		}
	}
}

CorpseCell* MouthCell::createCorpse() const
{
	return new CorpseCell(1.0f, localPos_);
}

void MouthCell::init()
{
}

float MouthCell::getMass() const
{
	return 1.0f;
}
