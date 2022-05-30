#include "pch.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "MoverCell.h"
#include "FruitCell.h"
#include "CorpseCell.h"
#include "MouthCell.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "Engine/Timer.h"
#include "Engine/Renderer2D.h"
#include "Engine/Util.h"
#include "OrganismManager.h"

Organism::Organism(std::string dna, const std::vector<Cell*>& cells, Vec2f position, float angle) :
	dna_(dna),
	cells_(cells)
{
	start_ = std::chrono::high_resolution_clock::now();
	Instances++;

	rigidBody_ = new RigidBody();
	rigidBody_->setRotation(angle);
	aabb_ = new AABB();
	aabb_->rigidBody = rigidBody_;

	Bounds bounds;
	constexpr float maxF = std::numeric_limits<float>::max();
	constexpr float minF = std::numeric_limits<float>::lowest();
	bounds.min = { maxF, maxF };
	bounds.max = { minF, minF };
	const float cellRadius = Cell::Size * 0.5f;

	for (Cell* cell : cells_)
	{
		cell->setOrganism(this);
		aabb_->colliders.push_back(cell->getCollider());

		Vec2f localPos = cell->getLocalPos();
		if (localPos.x < bounds.min.x)
			bounds.min.x = localPos.x - cellRadius;
		if (localPos.x > bounds.max.x)
			bounds.max.x = localPos.x + cellRadius;
		if (localPos.y < bounds.min.y)
			bounds.min.y = localPos.y - cellRadius;
		if (localPos.y > bounds.max.y)
			bounds.max.y = localPos.y + cellRadius;
	}	
	
	rigidBody_->setMass(cells_.size());
	rigidBody_->setPosition(position + (rigidBody_->getPosition() - bounds.center()));

	for (auto* cell : cells_)
		cell->init();
}

Organism::Organism(std::string dna, Vec2f position, float angle) :
	Organism(dna, getCellsForDNA(dna), position, angle)
{
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
	{
		if (cell->wantsToBeDeleted())
		{
			CircleCollider* collider = cell->getCollider();
			std::erase(aabb_->colliders, collider);
			delete collider;
			std::erase(cells_, cell);
			delete cell;
		}
	}

	if (cells_.size() == 0)
	{
		wantsToDie_ = true;
		aabb_->wantsToBeDeleted = true;
	}

	for (Cell* cell : cells_)
		cell->tick();
}

Organism* Organism::clone(Vec2f pos)
{
	return new Organism(dna_, getCellsForDNA(dna_), pos, Random::floatRange(0, 2 * std::numbers::pi));
}

Organism* Organism::createCorpse()
{
	std::vector<Cell*> corpseCells;
	for (const Cell* cell : cells_)
	{
		CorpseCell* corpse = cell->createCorpse();
		if (corpse)
			corpseCells.push_back(corpse);
	}

	if (corpseCells.size() == 0)
		return nullptr;

	return new Organism(dna_, corpseCells, rigidBody_->getPosition(), rigidBody_->getRotation());
}

AABB* Organism::getAABB()
{
	return aabb_;
}

unsigned Organism::getReproductionUrge()
{
	return reproductionCount_;
}

void Organism::setReproductionUrge(unsigned count)
{
	reproductionCount_ = count;
}

long Organism::getAgeMs()
{
	auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();
}

void Organism::markForDeath()
{
	wantsToDie_ = true;
}

float Organism::getMass()
{
	return rigidBody_->getMass();
}

void Organism::removeCell(Cell* cell)
{	
	cell->markForDeletion();
}

bool Organism::isMover() const
{
	return isMover_;
}

void Organism::setMover(bool isMover)
{
	isMover_ = isMover;
}

unsigned Organism::getSize()
{
	return cells_.size();
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

Cell* Organism::getCellForSymbol(char symbol)
{
	switch (symbol)
	{
	case 'L':
		return new LeafCell();
	case 'T':
		return new ThornCell();
	case 'M':
		return new MoverCell();
	case 'R':
		return new FruitCell();
	case 'O':
		return new MouthCell();
	default:
		throw std::exception(std::format("Invalid DNA symbol: {}", symbol).c_str());
	}
}

Vec2f Organism::getOffsetForParam(unsigned param)
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
	default:
		throw std::exception(std::format("Invalid DNA param: {}", param).c_str());
	}
}

std::vector<Cell*> Organism::getCellsForDNA(std::string dna)
{
	std::vector<Cell*> ret;

	Vec2f curPos = { 0, 0 };
	Vec2f offset = { 0, 0 };
	constexpr float maxF = std::numeric_limits<float>::max();
	Vec2f min = { maxF, maxF };
	constexpr float minF = std::numeric_limits<float>::lowest();
	Vec2f max = { minF, minF };

	for (size_t index = 0; index < dna.size(); index += 4)
	{
		const char symbol = dna[index];
		Cell* cell = getCellForSymbol(symbol);
		ret.push_back(cell);
		curPos += offset;
		cell->setLocalPos(curPos);
		unsigned param = dna[index + 2] - '0';
		offset = getOffsetForParam(param);
	}

	return ret;
}