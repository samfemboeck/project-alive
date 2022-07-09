#include "pch.h"
#include "ThornCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "LeafCell.h"
#include "MoverCell.h"
#include "HerbivoreCell.h"
#include "CorpseCell.h"

ThornCell::ThornCell() :
	Cell("cell_thorn.png")
{
	type_ = Type::Thorn;
}

void ThornCell::onCollision(Cell* other)
{	
	if (other->wantsToBeDeleted() || (other->getOrganism()->isPlant() && !other->getOrganism()->isCorpse()))
		return;

	if (other->getOrganism()->getSize() <= organism_->getSize())
	{
		other->getOrganism()->markForDeath();
		return;
	}
	else
	{
		organism_->markForDeath();
	}
}

CorpseCell* ThornCell::createCorpse() const
{
	return new CorpseCell(1.0f, localPos_, true);
}

char ThornCell::getSymbol()
{
	return 'T';
}
