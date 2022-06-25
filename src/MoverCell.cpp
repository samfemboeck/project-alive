#include "pch.h"
#include "MoverCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "OrganismManager.h"
#include "CorpseCell.h"

MoverCell::MoverCell() :
	Cell("cell_mover.png")
{
	type_ = Type::Mover;
}

MoverCell::~MoverCell()
{
	organism_->removeMover();
}

void MoverCell::init()
{
	organism_->addMover();
}

CorpseCell* MoverCell::createCorpse() const
{
	return new CorpseCell(1.0f, localPos_);
}

float MoverCell::getMass() const
{
	return 1.0f;
}

char MoverCell::getSymbol()
{
	return 'M';
}
