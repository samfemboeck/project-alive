#include "pch.h"
#include "LeafCell.h"
#include "Engine/Timer.h"
#include "Organism.h"

LeafCell::LeafCell() : 
	Cell("cell_leaf.png"),
	Timer(1000)
{
}

void LeafCell::tick(Organism* org)
{
	if (Timer.update())
	{
		org->m_timer_ttl.add_time(50 * LightLevel);
	}
}
