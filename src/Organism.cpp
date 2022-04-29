#include "pch.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "MoverCell.h"
#include "Engine/Physics.h"

bool Organism::overlaps_position(const glm::vec2& pos) const
{
	for (auto cell : m_cells)
	{
		if (glm::length(cell->LocalPos - pos) < Cell::Size)
		{
			return true;
		}
	}

	return false;
}

constexpr glm::vec2 Organism::get_offset_for_param(const unsigned param)
{
	switch (param)
	{
	case 0:
		return { 0, Cell::Size };
	case 1:
		return { Cell::Size, 0 };
	case 2:
		return { 0, -Cell::Size };
	case 3:
		return { -Cell::Size, 0 };
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

	b2BodyDef def;
	def.position.Set(0.0f, 0.0f);
	def.type = b2_dynamicBody;

	m_body = Physics::create_body(&def);

	glm::vec2 curPos = { 0, 0 };
	glm::vec2 offset = { 0, 0 };

	for (size_t index = 0; index < m_dna.size(); index += 4)
	{
		const char symbol = m_dna[index];
		Cell* const cell = get_cell_for_symbol(symbol);		
		m_cells.push_back(cell);

		curPos += offset;	

		cell->LocalPos = curPos;
		b2CircleShape circle_shape;
		circle_shape.m_radius = Cell::Size * 0.5f;
		circle_shape.m_p.Set(curPos.x, curPos.y);

		b2FixtureDef fix;
		fix.shape = &circle_shape;
		fix.density = 1.0f;
		fix.friction = 0.3f;
		m_body->CreateFixture(&fix);
		
		if (dynamic_cast<MoverCell*>(cell))
		{
			m_mover_cell = cell;
		}

		unsigned param = m_dna[index + 2] - '0';
		bool error = true;

		while (error)
		{
			offset = get_offset_for_param(param);

			if (!overlaps_position(curPos + offset))
				error = false;
			else
				param = (param + 1) % 4;
		}
	}

	if (m_mover_cell)
	{
		/*
		for (auto* cell : m_cells)
		{
			if (cell == m_mover_cell)
				continue;

			cell->Collider->Transform.Position = cell->Collider->Transform.Position - m_mover_cell->Collider->Transform.Position;
			cell->Collider->Transform.Parent = &m_mover_cell->Collider->Transform;
		}

		m_mover_cell->Collider->Transform.Position = {0, 0};
		*/
	}

	//m_body->SetLinearVelocity({ 100, 100 });
	std::reverse(m_cells.begin(), m_cells.end());
}

void Organism::tick(const float deltaTime)
{
}

void Organism::set_position(const glm::vec2& pos)
{
	m_body->SetTransform({pos.x, pos.y}, 0);
}

void Organism::set_velocity_linear(const glm::vec2& velocity)
{
	m_body->SetLinearVelocity({ velocity.x, velocity.y });
}

void Organism::set_velocity_angular(float vel)
{
	m_body->SetAngularVelocity(vel);
}

void Organism::draw() const
{
	auto* fix = m_body->GetFixtureList();
	for (int i = 0; fix; i++)
	{
		auto* circle_shape = dynamic_cast<b2CircleShape*>(fix->GetShape());
		auto angle = m_body->GetTransform().q.GetAngle();
		auto tf =
			glm::translate(glm::mat4(1.0f), { m_body->GetPosition().x, m_body->GetPosition().y, 0 }) *
			glm::rotate(glm::mat4(1.0f), angle, { 0, 0, 1 }) *
			glm::translate(glm::mat4(1.0f), { circle_shape->m_p.x, circle_shape->m_p.y, 0 }) *
			glm::scale(glm::mat4(1.0f), { circle_shape->m_radius * 2, circle_shape->m_radius * 2, 1 });
		auto pos = tf * glm::vec4(0, 0, 0, 1);
		log("cell: {0}, pos: {1}", m_cells[i]->TextureName, pos.y);

		Renderer2D::pushQuad(tf, TextureManager::get(m_cells[i]->TextureName), glm::vec4(1.0f), false);
		fix = fix->GetNext();
	}
}
