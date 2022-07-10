#include "pch.h"
#include "CarnivoreCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "LeafCell.h"
#include "MoverCell.h"
#include "HerbivoreCell.h"
#include "CorpseCell.h"

CarnivoreCell::CarnivoreCell() :
	Cell("cell_carnivore.png")
{
	type_ = Type::Carnivore;
}

void CarnivoreCell::onCollision(Cell* other)
{	
	if (other->wantsToBeDeleted() || (other->getOrganism()->isPlant() && !other->getOrganism()->isCorpse()))
		return;

	if (other->getOrganism()->isCorpse())
	{
		CorpseCell* corpse = dynamic_cast<CorpseCell*>(other);

		if (corpse)
		{
			other->getOrganism()->removeCell(corpse);
			float nutritionValue = other->getOrganism()->isMouth() ? corpse->getNutritionValue() * 1.4f : corpse->getNutritionValue();
			organism_->setEnergy(organism_->getEnergy() + nutritionValue);
		}
	}
	else if (other->getOrganism()->getSize() > organism_->getSize())
	{
		if (organism_->isMover() && other->getOrganism()->isMover())
		{
			return;
		}
		else
		{
			organism_->markForDeath();
			return;
		}
	}
	else if (other->getType() == Type::Carnivore)
	{
		if (other->getOrganism()->getSize() == organism_->getSize())
			return;

		if (other->getOrganism()->getSize() < organism_->getSize())
			other->getOrganism()->markForDeath();
		else
			organism_->markForDeath();
	}
	else if (!(other->getOrganism()->getDNA() == organism_->getDNA()))
	{
		if (other->getOrganism()->isMover())
			other->getOrganism()->markForDeath();
	}
}

CorpseCell* CarnivoreCell::createCorpse() const
{
	return new CorpseCell(1.0f, localPos_, false);
}

char CarnivoreCell::getSymbol()
{
	return 'C';
}
