#pragma once
#include "Cell.h"
#include "Engine/Timer.h"

struct LeafCell final : public Cell
{
	LeafCell();
	void tick(Organism* org) override;
	void on_collision(CircleCollider*) override;
	TickCountTimer Timer;
	unsigned LightLevel = 0;
	std::vector<Cell*> m_light_cells;
};