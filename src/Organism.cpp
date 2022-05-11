#include "pch.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "MoverCell.h"
#include "LightCell.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "Engine/Timer.h"

bool Organism::overlaps_position(const Vec2& pos) const
{
	for (auto* cell : m_cells)
	{
		if ((cell->LocalPos - pos).magnitude() < Cell::Size)
		{
			return true;
		}
	}

	return false;
}

Vec2 Organism::get_offset_for_param(const unsigned param) const
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

Cell* Organism::get_cell_for_symbol(const char symbol) const
{
	switch (symbol)
	{
	case 'L':
		return new LeafCell();
	case 'T':
		return new ThornCell();
	case 'M':
		return new MoverCell();
	case 'A':
		return new LightCell();
	}

	throw std::runtime_error("Invalid cell type.");
}

Organism::Organism(const std::string& dna, std::function<float(float)> instinct, Vec2 position, float angle) :
	m_dna(dna),
	m_initial_ttl(Organism::TTL),
	m_timer_ttl(m_initial_ttl),
	m_timer_clock(1000)
{
	m_rb = new RigidBody();
	m_rb->Position = position;
	PhysicsManager::getInstance().add(m_rb);

	Vec2 curPos = { 0, 0 };
	Vec2 offset = { 0, 0 };

	for (size_t index = 0; index < m_dna.size(); index += 4)
	{
		const char symbol = m_dna[index];
		Cell* cell = get_cell_for_symbol(symbol);		
		m_cells.push_back(cell);

		curPos += offset;	

		cell->LocalPos = curPos;
		CircleCollider* cc = new CircleCollider();
		cc->Radius = Cell::Size * 0.5f;
		cc->CenterLocal = curPos;
		cc->Body = m_rb;
		cc->Cell = cell;
		cc->CollisionCallback = std::bind(&Cell::on_collision, cell, std::placeholders::_1);
		m_colliders.push_back(cc);
		PhysicsManager::getInstance().add(cc);
		
		if (dynamic_cast<MoverCell*>(cell))
		{
			m_mover_cell = cell;
		}
		else if (dynamic_cast<LightCell*>(cell))
		{
			m_light_cell = cell;

			CircleCollider* cc = new CircleCollider();
			cc->IsSensor = true;
			cc->Radius = LightCell::Radius;
			cc->CenterLocal = curPos;
			cc->Body = m_rb;
			cc->Cell = nullptr;
			m_colliders.push_back(cc);
			PhysicsManager::getInstance().add(cc);
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

	std::reverse(m_cells.begin(), m_cells.end());

	m_instinct = instinct;
}

Organism::~Organism()
{
	for (auto coll : m_colliders)
		PhysicsManager::getInstance().remove(coll);

	PhysicsManager::getInstance().remove(m_rb);
}

int Organism::tick()
{
	if (m_light_cell)
		return -1;

	for (auto* cell : m_cells)
	{
		cell->tick(this);
	}

	if (m_mover_cell)
	{
		m_rb->AngularVelocity = (m_instinct(Time::ElapsedSeconds));
		Vec2 forward = b2Rot(m_rb->Rotation).GetYAxis();
		m_rb->Forces.push_back(forward);
	}

	bool is_life_over = m_timer_ttl.update();
	if (is_life_over)
	{
		if (m_timer_ttl.get_interval_ms() > m_initial_ttl)
			return 2;
		else
			return 0;
	}

	return -1;
}

void Organism::destroy()
{
	//Physics::destroy_body(m_body);
}

Organism* Organism::clone()
{
	return new Organism(m_dna, m_instinct, m_rb->Position + Random<Vec2>::range({ -50, -50 }, { 50, 50 }), Random<float>::range(0, 2 * std::numbers::pi));
}

void Organism::draw() const
{
	for (auto coll : m_colliders)
	{
		if (coll->Cell)
			Renderer2D::pushQuad(coll->Transform, TextureManager::get(coll->Cell->TextureName), glm::vec4(1.0f), false);
	}
}

void Organism::draw_light() const
{
	if (!m_light_cell)
		return;

	auto rbPos = glm::vec3{ m_rb->Position.X, m_rb->Position.Y, 0 };
	//auto angle = m_body->GetAngle();
	for (auto coll : m_colliders)
	{
		Cell* cell = coll->Cell;
		if (cell && dynamic_cast<LightCell*>(cell))
		{
			auto tf =
				glm::translate(glm::mat4(1.0f), rbPos) *
				glm::rotate(glm::mat4(1.0f), 0.0f, { 0, 0, 1 }) *
				glm::translate(glm::mat4(1.0f), { coll->CenterLocal.X, coll->CenterLocal.Y, 0 }) *
				glm::scale(glm::mat4(1.0f), { LightCell::Radius * 2, LightCell::Radius * 2, 1 });
			Renderer2D::pushQuad(tf, TextureManager::get("light_circle.png"), glm::vec4(1.0f), false);
		}
	}
}
