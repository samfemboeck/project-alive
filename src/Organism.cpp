#include "pch.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "MoverCell.h"
#include "LightCell.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "Engine/Timer.h"
#include "Engine/Renderer2D.h"

Organism::Organism(const std::string& dna, std::function<float(float)> instinct, Vec2f position_, float angle) :
	dna_(dna),
	initialTTL_(Organism::TimeToLive),
	timerTTL_(initialTTL_)
{
	Organism::ActiveInstances++;
	rigidBody_ = new RigidBody();
	rigidBody_->position = position_;
	PhysicsManager::getInstance().add(rigidBody_);

	Vec2f curPos = { 0, 0 };
	Vec2f offset = { 0, 0 };
	constexpr float maxF = std::numeric_limits<float>::max();
	Vec2f min = { maxF, maxF };
	constexpr float minF = std::numeric_limits<float>::min();
	Vec2f max = { minF, minF };

	for (size_t index = 0; index < dna_.size(); index += 4)
	{
		const char symbol = dna_[index];

		Cell* cell;
		CircleCollider* cc = new CircleCollider();

		switch (symbol)
		{
		case 'L':
			cell = new LeafCell(this, cc);
			break;
		case 'T':
			cell = new ThornCell(this, cc);
			break;
		case 'M':
			cell = new MoverCell(this, cc);
			break;
		case 'A':
			cell = new LightCell(this, cc);
			isLight_ = true;
			break;
		default:
			LOG("Invalid DNA symbol: {}", symbol);
		}

		cells_.push_back(cell);

		curPos += offset;
		if (curPos.x < min.x)
			min.x = curPos.x;
		if (curPos.x > max.x)
			max.x = curPos.x;
		if (curPos.y < min.y)
			min.y = curPos.y;
		if (curPos.y > max.y)
			max.y = curPos.y;

		cc->radius = Cell::Size * 0.5f;
		cc->centerLocal = curPos;
		cc->rigidBody = rigidBody_;
		cc->cell = cell;
		cc->collisionCallback = std::bind(&Cell::onCollision, cell, std::placeholders::_1);
		PhysicsManager::getInstance().add(cc);
		rigidBody_->colliders.push_back(cc);

		unsigned param = dna_[index + 2] - '0';

		switch (param)
		{
		case 0:
			offset = { 0, Cell::Size };
			break;
		case 1:
			offset = { Cell::Size, 0 };
			break;
		case 2:
			offset = { 0, -Cell::Size };
			break;
		case 3:
			offset = { -Cell::Size, 0 };
			break;
		default:
			LOG("Invalid DNA param: {}", param);
		}
	}

	auto* aabb = new AABB();
	float cellRadius = Cell::Size * 0.5f;
	min.x -= cellRadius;
	min.y -= cellRadius;
	max.x += cellRadius;
	max.y += cellRadius;
	aabb->boundsLocal = { min, max };
	aabb->rigidBody = rigidBody_;
	aabb_ = aabb;

	PhysicsManager::getInstance().add(aabb);

	instinct_ = instinct;
}

Organism::~Organism()
{
	PhysicsManager::getInstance().remove(rigidBody_);

	for (auto Cell : cells_)
		delete Cell;

	PhysicsManager::getInstance().remove(aabb_);
	Organism::ActiveInstances--;
}

int Organism::tick()
{
	for (auto* Cell : cells_)
		Cell->tick();

	if (isLight_)
		return -1;

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

Organism* Organism::clone()
{
	return new Organism(dna_, instinct_, rigidBody_->position + Random<Vec2f>::range({ -50, -50 }, { 50, 50 }), Random<float>::range(0, 2 * std::numbers::pi));
}

void Organism::draw() const
{
	for (auto* cell : cells_)
		cell->draw();

	Renderer2D::pushQuad(aabb_->boundsWorld.min, aabb_->boundsWorld.max, TextureManager::get("aabb.png"));
}