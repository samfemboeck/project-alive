#include "pch.h"
#include "ThornCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "LeafCell.h"
#include "MoverCell.h"
#include "MouthCell.h"
#include "CorpseCell.h"

ThornCell::ThornCell() :
	Cell("cell_thorn")
{
	type_ = Type::Thorn;
}

void ThornCell::onCollision(Cell* other)
{	
	if (other->wantsToBeDeleted() || (other->getOrganism()->isPlant() && !other->getOrganism()->isCorpse()))
		return;

	if (other->getOrganism()->isCorpse())
	{
		if (other->getOrganism()->isPredator()) // no cannibalism
			return;

		CorpseCell* corpse = dynamic_cast<CorpseCell*>(other);

		if (corpse)
		{
			other->getOrganism()->removeCell(corpse);
			float nutritionValue = other->getOrganism()->isMouth() ? corpse->getNutritionValue() * 1.5f : corpse->getNutritionValue();
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
	else if (other->getType() == Type::Thorn)
	{
		if (other->getOrganism()->getSize() == organism_->getSize())
			return;

		if (other->getOrganism()->getSize() < organism_->getSize())
		{
			other->getOrganism()->markForDeath();
		}
		else
		{
			organism_->markForDeath();
		}
	}
	else if (!(other->getOrganism()->getDNA() == organism_->getDNA()))
	{
		other->getOrganism()->markForDeath();
	}
}

CorpseCell* ThornCell::createCorpse() const
{
	return new CorpseCell(1.0f, localPos_);
}

void ThornCell::init()
{
}

void ThornCell::tick()
{
}
