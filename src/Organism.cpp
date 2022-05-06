#include "pch.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "MoverCell.h"
#include "LightCell.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"

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

Organism::Organism(const std::string& dna) :
	m_dna(dna),
	m_timer_ttl(10)
{
	b2BodyDef def;
	def.position.Set(0.0f, 0.0f);
	def.type = b2_staticBody;

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
			m_mover_cell = cell;

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
		data.mass = m_body->GetMass();
		m_body->SetMassData(&data);
		m_body->SetLinearDamping(1);
		m_body->SetAngularDamping(1);
		m_body->SetType(b2_dynamicBody);
		float rot = Random<float>::range(0, 2 * std::numbers::pi);
		LOG("Angle after init: {}", m_body->GetAngle());
	}

	float amplitude = Random<float>::range(0.25f, 3.0f);
	float offset_sin1 = Random<float>::range(0.0f, 10.0f);
	float multiplier_x_sin1 = Random<int>::range(1, 5);
	float offset_sin2 = Random<float>::range(0.0f, 10.0f);
	float multiplier_x_sin2 = Random<int>::range(1, 5);

	m_instinct = [=](float x) -> float { 
		return amplitude * (sin(multiplier_x_sin1 * x + offset_sin1) + sin(multiplier_x_sin2 * x + offset_sin2)); 
	};
	m_velocity = Random<int>::range(1, 100);
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

void Organism::tick()
{
	if (!m_mover_cell)
		return;

	auto rot = m_body->GetTransform().q.GetAngle();
	m_body->SetAngularVelocity(m_instinct(Time::Elapsed));
	Vec2 forward = m_body->GetTransform().q.GetYAxis();
	m_body->ApplyForceToCenter(forward * 1000 * m_velocity, true);
}

void Organism::set_transform(Vec2 pos, float rot)
{
	m_body->SetTransform(pos, rot);
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
		auto* cell = m_cells[i];
		Renderer2D::pushQuad(tf, TextureManager::get(cell->TextureName), glm::vec4(1.0f), false);
		auto* light_cell = dynamic_cast<LightCell*>(cell);
		if (light_cell)
		{
			auto tf =
				glm::translate(glm::mat4(1.0f), rbPos) *
				glm::rotate(glm::mat4(1.0f), angle, { 0, 0, 1 }) *
				glm::translate(glm::mat4(1.0f), { circle_shape->m_p.x, circle_shape->m_p.y, 0 }) *
				glm::scale(glm::mat4(1.0f), { light_cell->Radius * 2, light_cell->Radius * 2, 1 });
			Renderer2D::pushQuad(tf, TextureManager::get("light_circle.png"), glm::vec4(1.0f), false);
		}
		fix = fix->GetNext();
	}
}