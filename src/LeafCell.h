#pragma once
#include "Cell.h"
#include "Engine/Timer.h"

struct LeafCell final : public Cell
{
	LeafCell();
	void tick(Organism* org) override;
	void onCollision(CircleCollider*) override;
	TickCountTimer timer;
	unsigned lightLevel = 0;
	std::vector<Cell*> lightCells;
};