#include "pch.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "MoverCell.h"
#include "LightCell.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "Engine/Timer.h"

Organism::Organism(const std::string& dna, std::function<float(float)> instinct, Vec2 position_, float angle) :
	dna_(dna),
	initialTTL_(Organism::TimeToLive),
	timerTTL_(initialTTL_)
{
	Organism::ActiveInstances++;
	rigidBody_ = new RigidBody();
	rigidBody_->position = position_;
	PhysicsManager::getInstance().add(rigidBody_);

	Vec2 curPos = { 0, 0 };
	Vec2 offset = { 0, 0 };

	for (size_t index = 0; index < dna_.size(); index += 4)
	{
		const char symbol = dna_[index];
		Cell* cell = getCellForSymbol(symbol);		
		cells_.push_back(cell);

		curPos += offset;	

		cell->localPos = curPos;
		CircleCollider* cc = new CircleCollider();
		cc->radius = Cell::Size * 0.5f;
		cc->centerLocal = curPos;
		cc->body = rigidBody_;
		cc->cell = cell;
		cc->collisionCallback = std::bind(&Cell::onCollision, cell, std::placeholders::_1);
		colliders_.push_back(cc);
		PhysicsManager::getInstance().add(cc);
		
		if (dynamic_cast<MoverCell*>(cell))
		{
			moverCell_ = cell;
		}
		else if (dynamic_cast<LightCell*>(cell))
		{
			lightCell_ = cell;

			CircleCollider* cc = new CircleCollider();
			cc->isSensor = true;
			cc->radius = LightCell::Radius;
			cc->centerLocal = curPos;
			cc->body = rigidBody_;
			cc->cell = nullptr;
			colliders_.push_back(cc);
			PhysicsManager::getInstance().add(cc);
		}

		unsigned param = dna_[index + 2] - '0';
		bool error = true;

		while (error)
		{
			offset = getOffsetForParam(param);

			if (!overlapsPosition(curPos + offset))
				error = false;
			else
				param = (param + 1) % 4;
		}
	}

	std::reverse(cells_.begin(), cells_.end());

	instinct_ = instinct;
}

Organism::~Organism()
{
	for (auto coll : colliders_)
		PhysicsManager::getInstance().remove(coll);

	PhysicsManager::getInstance().remove(rigidBody_);

	for (auto Cell : cells_)
		delete Cell;

	Organism::ActiveInstances--;
}

int Organism::tick()
{
	if (lightCell_)
		return -1;

	for (auto* Cell : cells_)
	{
		Cell->tick(this);
	}

	if (moverCell_)
	{
		//m_rb->AngularVelocity = (m_instinct(Time::ElapsedSeconds));
		Vec2 forward = b2Rot(rigidBody_->rotation).GetYAxis();
		rigidBody_->forces.push_back(forward);
	}

	bool isLifeOver = timerTTL_.update();
	if (isLifeOver)
	{
		if (timerTTL_.getIntervalMs() > initialTTL_)
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
	return new Organism(dna_, instinct_, rigidBody_->position + Random<Vec2>::range({ -50, -50 }, { 50, 50 }), Random<float>::range(0, 2 * std::numbers::pi));
}

void Organism::draw() const
{
	for (auto coll : colliders_)
	{
		if (coll->cell && !dynamic_cast<LightCell*>(coll->cell))
			Renderer2D::pushQuad(coll->transform, TextureManager::get(coll->cell->textureName), glm::vec4(1.0f), false);
	}
}

bool Organism::overlapsPosition(const Vec2& pos) const
{
	for (auto* Cell : cells_)
	{
		if ((Cell->localPos - pos).magnitude() < Cell::Size)
		{
			return true;
		}
	}

	return false;
}

Vec2 Organism::getOffsetForParam(const unsigned param) const
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

Cell* Organism::getCellForSymbol(const char symbol) const
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
