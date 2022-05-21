#include "pch.h"
#include "LeafCell.h"
#include "Organism.h"
#include "Engine/Physics.h"

LeafCell::LeafCell(Organism* org, CircleCollider* collider) : 
	Cell(org, collider, "cell_leaf"),
	timerAddTTL_(1000)
{
}

void LeafCell::tick()
{
	if (!isLit_)
		return;

	if (timerAddTTL_.update())
	{
		organism_->timerTTL_.addTime(500);
	}
}

void LeafCell::onCollision(Cell* other)
{
}

void LeafCell::setLit(bool isLit)
{
	isLit_ = isLit;
}
