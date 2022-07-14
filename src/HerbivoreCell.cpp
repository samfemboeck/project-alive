#include "HerbivoreCell.h"
#include "CorpseCell.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "pch.h"

HerbivoreCell::HerbivoreCell() :
	Cell("cell_herbivore.png")
{
	type_ = Type::Herbivore;
}

void HerbivoreCell::onCollision(Cell* other)
{
	if (other->wantsToBeDeleted())
		return;

	LeafCell* leafCell = dynamic_cast<LeafCell*>(other);
	if (leafCell)
	{
		organism_->setEnergy(organism_->getEnergy() + leafCell->getNutritionValue());
		other->getOrganism()->removeCell(leafCell);
		other->getOrganism()->markForDeath();
	}
	else
	{
		CorpseCell* corpseCell = dynamic_cast<CorpseCell*>(other);
		if (corpseCell)
		{
			if (corpseCell->isPlant())
				organism_->setEnergy(organism_->getEnergy() + corpseCell->getNutritionValue());
			else
				organism_->setEnergy(organism_->getEnergy() + (corpseCell->getNutritionValue() * 0.5f));
			other->getOrganism()->removeCell(corpseCell);
		}
	}
}

CorpseCell* HerbivoreCell::createCorpse() const
{
	return new CorpseCell(1.0f, localPos_, false);
}

void HerbivoreCell::init()
{
}

float HerbivoreCell::getMass() const
{
	return 0.10f;
}

char HerbivoreCell::getSymbol()
{
	return 'O';
}
