#include "pch.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "MoverCell.h"
#include "Engine/Physics.h"

bool Organism::overlaps_position(const Vec2& pos) const
{
	for (auto cell : m_cells)
	{
		if (QuickMaths::mag(cell->LocalPos - pos) < Cell::Size)
		{
			return true;
		}
	}

	return false;
}

constexpr Vec2 Organism::get_offset_for_param(const unsigned param)
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
	m_dna(dna),
	m_timer(1)
{
	log(dna);

	b2BodyDef def;
	def.position.Set(0.0f, 0.0f);
	def.type = b2_dynamicBody;

	m_body = Physics::create_body(&def);

	Vec2 curPos = { 0, 0 };
	Vec2 offset = { 0, 0 };

	for (size_t index = 0; index < m_dna.size(); index += 4)
	{
		const char symbol = m_dna[index];
		Cell* const cell = get_cell_for_symbol(symbol);		
		m_cells.push_back(cell);

		curPos += offset;	

		cell->LocalPos = curPos;
		b2CircleShape circle_shape;
		circle_shape.m_radius = Cell::Size * 0.5f;
		circle_shape.m_p.Set(curPos.X, curPos.Y);

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

	std::reverse(m_cells.begin(), m_cells.end());

	if (m_mover_cell)
	{
		b2MassData data{};
		data.center = m_mover_cell->LocalPos;
		data.mass = 1;
		data.I = 100000;
		m_body->SetMassData(&data);
		m_body->SetLinearDamping(1);
	}
}

void Organism::tick(const float deltaTime)
{
	if (!m_mover_cell)
		return;

	auto y = m_body->GetTransform().q.GetYAxis();
	auto x = m_body->GetTransform().q.GetXAxis();
	auto dir = m_body->GetPosition() - m_target_point;

	if (QuickMaths::mag(dir) < 50)
		m_target_point = QuickMaths::rand_vec({ -400, -400 }, { 400, 400 });

	m_speed_linear = dir.Normalize();
	m_body->SetLinearVelocity({y.x * m_speed_linear, y.y * m_speed_linear});
	m_body->SetAngularVelocity(b2Dot(x, dir) * m_speed_angular);
}

void Organism::set_position(const Vec2& pos)
{
	m_body->SetTransform(pos, 0);
}

void Organism::set_velocity_linear(const Vec2& velocity)
{
	if (!m_mover_cell)
		return;

	m_body->SetLinearVelocity(velocity);
}

void Organism::set_velocity_angular(float vel)
{
	if (!m_mover_cell)
		return;

	m_body->SetAngularVelocity(vel);
}

void Organism::draw() const
{
	auto* fix = m_body->GetFixtureList();
	for (int i = 0; fix; i++)
	{
		auto* circle_shape = dynamic_cast<b2CircleShape*>(fix->GetShape());
		auto angle = m_body->GetTransform().q.GetAngle();
		auto rbPos = glm::vec3{ m_body->GetPosition().x, m_body->GetPosition().y, 0 };
		auto tf =
			glm::translate(glm::mat4(1.0f), rbPos) *
			glm::rotate(glm::mat4(1.0f), angle, { 0, 0, 1 }) *
			glm::translate(glm::mat4(1.0f), { circle_shape->m_p.x, circle_shape->m_p.y, 0 }) *
			glm::scale(glm::mat4(1.0f), { circle_shape->m_radius * 2, circle_shape->m_radius * 2, 1 });
		auto pos = tf * glm::vec4(0, 0, 0, 1);
		Renderer2D::pushQuad(tf, TextureManager::get(m_cells[i]->TextureName), glm::vec4(1.0f), false);
		fix = fix->GetNext();
	}
}
