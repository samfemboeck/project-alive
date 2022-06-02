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
#include "Mutation.h"

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
		if (dynamic_cast<MoverCell*>(cell))
			isMover_ = true;
		
		if (dynamic_cast<CorpseCell*>(cell))
			isCorpse_ = true;

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

	if (isMover_)
	{
		long ttl = getSize() * 2500;
		ttl_ = Random::floatRange(ttl, ttl + 1000);
	}
	else
	{
		ttl_ = Random::floatRange(10000, 30000);
	}

	hunger_ = cells_.size() * 34.0f + 68.0f;

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
	if (!isCorpse_ && getAgeMs() >= ttl_)
		markForDeath();

	for (auto it = cells_.begin(); it != cells_.end();)
	{
		Cell* cell = *it;
		if (cell->wantsToBeDeleted())
		{
			CircleCollider* collider = cell->getCollider();
			std::erase(aabb_->colliders, collider);
			delete collider;
			it = cells_.erase(it);
			delete cell;
		}
		else
			it++;
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
	std::string dna = Production::produce(dna_);
	return new Organism(dna, getCellsForDNA(dna), pos, Random::floatRange(0, 2 * std::numbers::pi));
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

unsigned Organism::getSize()
{
	return cells_.size();
}

void Organism::addEnergy(float energy)
{
	energy_ += energy;
	if (energy_ >= hunger_)
	{
		reproductionCount_ += 1;
		energy_ = 0.0f;
	}
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
	case 'P':
		return new LeafCell();
	case 'T':
		return new ThornCell();
	case 'M':
		return new MoverCell();
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

	std::vector<Vec2f> savedPositions;
	std::vector<float> savedRotations;
	int stackIdx = -1;
	Vec2f curPos = { 0, 0 };
	float curRot = 0;
	std::string cells = "PTMO";
	char activeCell;

	activeCell = dna[0];
	Cell* cell = getCellForSymbol(activeCell);
	cell->setLocalPos({0, 0});
	ret.push_back(cell);

	for (size_t index = 4; index < dna.size();)
	{
		const char instruction = dna[index];

		if (cells.find(instruction) != std::string::npos)
		{
			activeCell = instruction;
			index += 4;
		}
		else if (instruction == 'F')
		{
			Vec2f offset = { sin(curRot), cos(curRot) };
			unsigned param = dna[index + 2] - '0';

			for (unsigned i = 0; i < param; i++)
			{
				curPos += offset * Cell::Size;
				bool found = false;

				for (Cell* cell : ret)
					if (cell->getLocalPos() == curPos)
						found = true;

				if (!found)
				{
					Cell* cell = getCellForSymbol(activeCell);
					cell->setLocalPos(curPos);
					ret.push_back(cell);
				}
			}

			index += 6;
		}
		else if (instruction == 'L')
		{
			unsigned param = dna[index + 2] - '0';
			curRot -= param * 0.5f * std::numbers::pi;
			index += 6;
		}
		else if (instruction == 'R')
		{
			unsigned param = dna[index + 2] - '0';
			curRot += param * 0.5f * std::numbers::pi;
			index += 6;
		}
		else if (instruction == '[')
		{
			savedPositions.push_back(curPos);
			savedRotations.push_back(curRot);
			stackIdx++;
			index += 1;
		}
		else if (instruction == ']')
		{
			curPos = savedPositions[stackIdx];
			curRot = savedRotations[stackIdx];
			savedPositions.pop_back();
			savedRotations.pop_back();
			stackIdx--;
			index += 1;
		}
	}

	return ret;
}