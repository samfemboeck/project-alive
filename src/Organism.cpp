#include "pch.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "MoverCell.h"
#include "FruitCell.h"
#include "CorpseCell.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "Engine/Timer.h"
#include "Engine/Renderer2D.h"
#include "Engine/Util.h"
#include "OrganismManager.h"

Organism::Organism(const std::string& dna__, Vec2f position__, float angle__) :
	dna_(dna__)
{
	start_ = std::chrono::high_resolution_clock::now();
	Instances++;

	rigidBody_ = new RigidBody();
	rigidBody_->rotation = angle__;
	aabb_ = new AABB();
	aabb_->rigidBody = rigidBody_;

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
			isLeaf_ = true;
			break;
		case 'T':
			cell = new ThornCell(this, cc);
			break;
		case 'M':
			cell = new MoverCell(this, cc);
			isMover_ = true;
			break;
		case 'R':
			cell = new FruitCell(this, cc);
			break;
		case 'X':
			cell = new CorpseCell(this, cc);
			break;
		default:
			throw std::exception(std::format("Invalid DNA symbol: {}", symbol).c_str());
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
		aabb_->colliders.push_back(cc);

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
			throw std::exception(std::format("Invalid DNA param: {}", symbol).c_str());
		}
	}

	const float cellRadius = Cell::Size * 0.5f;
	min.x -= cellRadius;
	min.y -= cellRadius;
	max.x += cellRadius;
	max.y += cellRadius;
	Bounds bounds(min, max);
	offsetCenterToRb_ = rigidBody_->position - bounds.center();
	setPosition(position__);

	for (auto* cell : cells_)
		cell->init();
}

Organism::~Organism()
{
	Instances--;

	for (Cell* Cell : cells_)
		delete Cell;

	for (CircleCollider* collider : aabb_->colliders)
		delete collider;

	delete rigidBody_;

	aabb_->wantsToBeDeleted = true;
}

void Organism::tick()
{
	for (Cell* cell : cells_)
		cell->tick();
}

Organism* Organism::clone(Vec2f pos)
{
	return new Organism(dna_, pos, Random::floatRange(0, 2 * std::numbers::pi));
}

Organism* Organism::createCorpse()
{
	std::string dna = dna_;
	dna = Util::replaceAll(dna, "L", "X");
	dna = Util::replaceAll(dna, "T", "X");
	dna = Util::replaceAll(dna, "M", "X");
	dna = Util::replaceAll(dna, "R", "X");
	auto* ret = new Organism(dna, rigidBody_->position, rigidBody_->rotation);
	ret->isCorpse_ = true;
	return ret;
}

AABB* Organism::getAABB()
{
	return aabb_;
}

void Organism::setPosition(Vec2f pos)
{
	rigidBody_->position = pos + offsetCenterToRb_;
}

bool Organism::wantsToBeDeleted()
{
	return wantsToBeDeleted_;
}

bool Organism::isMover()
{
	return isMover_;
}

bool Organism::isLeaf()
{
	return isLeaf_;
}

bool Organism::isCorpse()
{
	return isCorpse_;
}

unsigned Organism::getReproductionCount()
{
	return reproductionCount_;
}

void Organism::setReproductionCount(unsigned count)
{
	reproductionCount_ = count;
}

long Organism::getAgeMs()
{
	auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();
}

void Organism::markForDeletion()
{
	wantsToBeDeleted_ = true;
}

void Organism::markForDeath()
{
	wantsToDie_ = true;
}

RigidBody* Organism::getRigidBody()
{
	return rigidBody_;
}

bool Organism::wantsToDie()
{
	return wantsToDie_;
}

void Organism::draw() const
{
	for (auto* cell : cells_)
		cell->draw();

	//Renderer2D::pushQuad(aabb_->bounds.min, aabb_->bounds.max, TextureManager::get("aabb.png"));
}