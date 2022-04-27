#include "pch.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "MoverCell.h"

bool Organism::overlaps_position(const glm::vec2& pos) const
{
	for (auto cell : m_cells)
	{
		if (glm::length(cell->m_transform.Position - pos) < Cell::s_size)
		{
			return true;
		}
	}

	return false;
}

constexpr glm::vec2 Organism::get_offset_for_param(const unsigned int param)
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

constexpr Cell* Organism::get_cell_for_symbol(const char symbol)
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

Organism::Organism(const std::string& dna) :
	m_dna(dna)
{
	log(dna);
	glm::vec2 curPos = m_rigid_body.Transform.Position;
	glm::vec2 offset = { 0, 0 };
	
	for (int index = 0; index < m_dna.size(); index += 4)
	{
		const char symbol = m_dna[index];
		Cell* const cell = get_cell_for_symbol(symbol);
		m_cells.push_back(cell);
		curPos += offset;
		cell->m_transform.Position = curPos;

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

			const glm::vec2 offset = cell->m_transform.Position - m_mover_cell->m_transform.Position;
			cell->m_transform.Parent = &m_mover_cell->m_transform;
			cell->m_transform.Position = offset;
			cell->m_transform.Scale = { 1, 1 };
		}
	}

	m_circle_collider = new CircleCollider(&m_rigid_body, Cell::s_size * 0.5f);
}

void Organism::tick(const float deltaTime)
{
	if (m_mover_cell)
	{
		m_mover_cell->m_transform.Position = m_rigid_body.Transform.Position;
		m_mover_cell->m_transform.Rotation = m_rigid_body.Transform.Rotation;
	}
}

void Organism::set_position(const glm::vec2& pos)
{
	m_rigid_body.Transform.Position = pos;
}

void Organism::set_velocity(const glm::vec2& velocity)
{
	m_rigid_body.Velocity = velocity;
}

void Organism::draw() const
{
	for (Cell* cell : m_cells)
		cell->draw();
}
