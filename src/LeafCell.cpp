#include "pch.h"
#include "LeafCell.h"
#include "Engine/Timer.h"
#include "Organism.h"
#include "LightCell.h"
#include "Engine/Physics.h"

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

void LeafCell::on_collision(CircleCollider* other)
{
	if (other->Cell == nullptr && std::find(m_light_cells.begin(), m_light_cells.end(), other->Cell) == m_light_cells.end())
	{
		m_light_cells.push_back(other->Cell);
		LightLevel++;
	}
}
