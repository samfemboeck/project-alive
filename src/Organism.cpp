#include "pch.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "MoverCell.h"

bool Organism::overlaps_position(glm::vec2 pos)
{
	for (auto cell : m_cells)
	{
		if (glm::length(cell->m_transform.pm_position - pos) < Cell::s_size)
		{
			return true;
		}
	}

	return false;
}

glm::vec2 Organism::get_offset_for_param(unsigned int param)
{
	switch (param)
	{
	case 0:
		return { 0, Cell::s_size };
	case 1:
		return { Cell::s_size, 0 };
	case 2:
		return { 0, -Cell::s_size };
	case 3:
		return { -Cell::s_size, 0 };
	}

	throw std::runtime_error("Invalid parameter.");
}

Cell* Organism::get_cell_for_symbol(char symbol)
{
	switch (symbol)
	{
	case 'L':
		return new LeafCell();
	case 'T':
		return new ThornCell();
	case 'M':
		return new MoverCell();
	}

	throw std::runtime_error("Invalid cell type.");
}

Organism::Organism(std::string dna) :
	m_dna(dna)
{
	log(dna);
	glm::vec2 curPos = m_rigid_body.pm_transform.pm_position;
	glm::vec2 offset = { 0, 0 };
	
	for (int index = 0; index < m_dna.size(); index += 4)
	{
		char symbol = m_dna[index];
		Cell* cell = get_cell_for_symbol(symbol);
		m_cells.push_back(cell);
		curPos += offset;
		cell->m_transform.pm_position = curPos;

		if (dynamic_cast<MoverCell*>(cell))
			m_mover_cell = cell;

		unsigned int param = m_dna[index + 2] - '0';
		bool error = true;

		while (error)
		{
			offset = get_offset_for_param(param);

			if (overlaps_position(curPos + offset))
			{
				param = (param + 1) % 4;
			}
			else
			{
				error = false;
			}
		}
	}

	if (m_mover_cell)
	{
		for (Cell* cell : m_cells)
		{
			if (cell == m_mover_cell)
				continue;

			glm::vec2 offset = cell->m_transform.pm_position - m_mover_cell->m_transform.pm_position;
			cell->m_transform.pm_parent = &m_mover_cell->m_transform;
			cell->m_transform.pm_position = offset;
			cell->m_transform.pm_scale = { 1, 1 };
		}
	}

	m_circle_collider = new CircleCollider(&m_rigid_body, Cell::s_size * 0.5f);
}

void Organism::tick(float deltaTime)
{
	auto pos = m_rigid_body.pm_transform.pm_position;
	auto rot = m_rigid_body.pm_transform.pm_rotation;
	if (m_mover_cell)
	{
		m_mover_cell->m_transform.pm_position = m_rigid_body.pm_transform.pm_position;
		m_mover_cell->m_transform.pm_rotation = m_rigid_body.pm_transform.pm_rotation;
	}
}

void Organism::draw()
{
	for (Cell* cell : m_cells)
		cell->draw();
}
