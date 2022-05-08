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
	m_timer_ttl(m_initial_ttl)
{
	b2BodyDef def;
	def.position.Set(position.X, position.Y);
	def.angle = angle;
	def.type = b2_dynamicBody;

	m_body = Physics::create_body(&def);

	Vec2 curPos = { 0, 0 };
	Vec2 offset = { 0, 0 };

	for (size_t index = 0; index < m_dna.size(); index += 4)
	{
		const char symbol = m_dna[index];
		Cell* cell = get_cell_for_symbol(symbol);		
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
		fix.isSensor = false;
		fix.userData.pointer = reinterpret_cast<uintptr_t>(cell);

		m_body->CreateFixture(&fix);
		
		if (dynamic_cast<MoverCell*>(cell))
		{
			m_mover_cell = cell;
		}
		else if (dynamic_cast<LightCell*>(cell))
		{
			m_light_cell = cell;

			b2CircleShape circle_shape;
			circle_shape.m_radius = LightCell::Radius;
			circle_shape.m_p.Set(curPos.X, curPos.Y);
			
			b2FixtureDef fix;
			fix.shape = &circle_shape;
			fix.density = 1.0f;
			fix.friction = 0.3f;
			fix.isSensor = true;
			fix.userData.pointer = 0;

			m_body->CreateFixture(&fix);
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
		data.mass = m_body->GetMass();
		m_body->SetMassData(&data);
		m_body->SetLinearDamping(1);
		m_body->SetAngularDamping(1);
	}

	m_instinct = instinct;
}

Organism::~Organism()
{
	Physics::destroy_body(m_body);
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
		m_body->SetAngularVelocity(m_instinct(Time::ElapsedSeconds));
		Vec2 forward = m_body->GetTransform().q.GetYAxis();
		m_body->ApplyForceToCenter(forward * 1000 * Random<int>::range(1, 100), true);
	}

	bool is_life_over = m_timer_ttl.update();
	if (is_life_over)
	{
		if (m_timer_ttl.get_interval_ms() > m_initial_ttl)
			return rand() % 2 + 1;
		else
			return 0;
	}

	return -1;
}

void Organism::set_transform(Vec2 pos, float rot)
{
	m_body->SetTransform(pos, rot);
}

void Organism::destroy()
{
	Physics::destroy_body(m_body);
}

Organism* Organism::clone()
{
	return new Organism(m_dna, m_instinct, m_body->GetPosition() + Random<Vec2>::range({ -100, -100 }, { 100, 100 }), Random<float>::range(0, 2 * std::numbers::pi));
}

void Organism::draw() const
{
	auto* fix = m_body->GetFixtureList();
	for (; fix; fix = fix->GetNext())
	{
		auto* circle_shape = dynamic_cast<b2CircleShape*>(fix->GetShape());
		auto angle = m_body->GetAngle();
		auto rbPos = glm::vec3{ m_body->GetPosition().x, m_body->GetPosition().y, 0 };
		auto tf =
			glm::translate(glm::mat4(1.0f), rbPos) *
			glm::rotate(glm::mat4(1.0f), angle, { 0, 0, 1 }) *
			glm::translate(glm::mat4(1.0f), { circle_shape->m_p.x, circle_shape->m_p.y, 0 }) *
			glm::scale(glm::mat4(1.0f), { circle_shape->m_radius * 2, circle_shape->m_radius * 2, 1 });
		auto pos = tf * glm::vec4(0, 0, 0, 1);
		auto* cell = reinterpret_cast<Cell*>(fix->GetUserData().pointer);
		if (cell)
			Renderer2D::pushQuad(tf, TextureManager::get(cell->TextureName), glm::vec4(1.0f), false);
	}
}

void Organism::draw_light() const
{
	if (!m_light_cell)
		return;

	auto rbPos = glm::vec3{ m_body->GetPosition().x, m_body->GetPosition().y, 0 };
	auto angle = m_body->GetAngle();
	auto* fix = m_body->GetFixtureList();
	for (int i = 0; fix; fix = fix->GetNext(), i++)
	{
		auto& user_data = fix->GetUserData();
		Cell* cell = reinterpret_cast<Cell*>(user_data.pointer);
		if (cell && dynamic_cast<LightCell*>(cell))
		{
			auto* circle_shape = dynamic_cast<b2CircleShape*>(fix->GetShape());
			auto tf =
				glm::translate(glm::mat4(1.0f), rbPos) *
				glm::rotate(glm::mat4(1.0f), angle, { 0, 0, 1 }) *
				glm::translate(glm::mat4(1.0f), { circle_shape->m_p.x, circle_shape->m_p.y, 0 }) *
				glm::scale(glm::mat4(1.0f), { LightCell::Radius * 2, LightCell::Radius * 2, 1 });
			Renderer2D::pushQuad(tf, TextureManager::get("light_circle.png"), glm::vec4(1.0f), false);
		}
	}
}
