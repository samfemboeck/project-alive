#pragma once
#include "Cell.h"
#include "Engine/Timer.h"

struct LeafCell final : public Cell
{
	LeafCell();
	void tick(Organism* org) override;
	TickCountTimer Timer;
	unsigned LightLevel = 0;
};