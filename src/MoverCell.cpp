#include "pch.h"
#include "MoverCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "OrganismManager.h"
#include "CorpseCell.h"

MoverCell::MoverCell() :
	Cell("cell_mover")
{
}

MoverCell::~MoverCell()
{
	organism_->removeMover();
}

void MoverCell::init()
{
	organism_->addMover();
}

void MoverCell::tick()
{
}

CorpseCell* MoverCell::createCorpse() const
{
	return new CorpseCell(0.5f, localPos_);
}