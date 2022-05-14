#include "pch.h"
#include "LeafCell.h"
#include "Engine/Timer.h"
#include "Organism.h"
#include "LightCell.h"
#include "Engine/Physics.h"

LeafCell::LeafCell() : 
	Cell("cell_leaf.png"),
	timer(1000)
{
}

void LeafCell::tick(Organism* org)
{
	if (timer.update())
	{
		org->timerTTL_.addTime(50 * lightLevel);
	}
}

void LeafCell::onCollision(CircleCollider* other)
{
	if (other->cell == nullptr && std::find(lightCells.begin(), lightCells.end(), other->cell) == lightCells.end())
	{
		lightCells.push_back(other->cell);
		lightLevel++;
	}
}
