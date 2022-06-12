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
}

void ThornCell::onCollision(Cell* other)
{	
	if (other->wantsToBeDeleted())
		return;

	MoverCell* moverCell = dynamic_cast<MoverCell*>(other);
	if (moverCell)
	{
		other->getOrganism()->markForDeath();
	}
	else
	{
		MouthCell* mouthCell = dynamic_cast<MouthCell*>(other);
		{
			if (mouthCell)
			{
				other->getOrganism()->markForDeath();
			}
			else
			{
				ThornCell* thornCell = dynamic_cast<ThornCell*>(other);
				if (thornCell)
				{
					if (other->getOrganism()->getMass() == organism_->getMass())
						return;

					if (other->getOrganism()->getMass() < organism_->getMass())
						other->getOrganism()->removeCell(other);
					else
						getOrganism()->removeCell(this);
				}
			}
		}
	}
}

CorpseCell* ThornCell::createCorpse() const
{
	return new CorpseCell(0.5f, localPos_);
}

void ThornCell::init()
{
}

void ThornCell::tick()
{	
}
