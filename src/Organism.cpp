#include "pch.h"
#include "Organism.h"
#include "LeafCell.h"
#include "ThornCell.h"
#include "MoverCell.h"
#include "CorpseCell.h"
#include "HerbivoreCell.h"
#include "CarnivoreCell.h"
#include "Engine/Physics.h"
#include "Engine/Time.h"
#include "Engine/Timer.h"
#include "Engine/Renderer2D.h"
#include "Engine/Util.h"
#include "OrganismManager.h"
#include "DNA.h"

Organism::Organism(DNA dna, const std::vector<Cell*>& cells, Vec2f position, float angle) :
	dna_(dna),
	cells_(cells)
{
	start_ = std::chrono::high_resolution_clock::now();
	Instances++;

	rigidBody_ = new RigidBody();
	rigidBody_->setRotation(angle);
	aabb_ = new AABB();
	aabb_->organism = this;
	aabb_->rigidBody = rigidBody_;

	Bounds bounds;
	constexpr float maxF = std::numeric_limits<float>::max();
	constexpr float minF = std::numeric_limits<float>::lowest();
	bounds.min = { maxF, maxF };
	bounds.max = { minF, minF };
	const float cellRadius = Cell::Size * 0.5f;

	unsigned numHerbivoreCells = 0;
	unsigned numCarnivoreCells = 0;

	for (Cell* cell : cells_)
	{
		switch (cell->getType())
		{
			case Type::Mover:
				setMover(true);
				rigidBody_->setLinearFriction(1.0f);
				break;
			case Type::Herbivore:
				isHerbivore_ = true;
				numHerbivoreCells++;
				break;
			case Type::Plant:
				isPlant_ = true;
				break;
			case Type::Thorn:
				isThorn_ = true;
				break;
			case Type::Corpse:
				isCorpse_ = true;
				break;
			case Type::Carnivore:
				isCarnivore_ = true;
				numCarnivoreCells++;
				break;
		}
	
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

	Vec2f center = bounds.center();
	Cell* minCell = nullptr;
	float minDistance = maxF;

	for (Cell* cell : cells_)
	{
		float magS = (center - cell->getLocalPos()).magnitude_squared();
		if (magS < minDistance)
		{
			minDistance = magS;
			minCell = cell;
		}
	}

	if (minCell)
	{
		for (Cell* cell : cells_)
		{
			if (cell == minCell)
				continue;

			cell->setLocalPos(cell->getLocalPos() - minCell->getLocalPos());
			cell->setOrganism(this);
			aabb_->colliders.push_back(cell->getCollider());
		}
	}

	minCell->setLocalPos({ 0, 0 });
	minCell->setOrganism(this);
	aabb_->colliders.push_back(minCell->getCollider());

	if (isMover_)
		ttl_ = Random::floatRange(16.0f, 19.0f);
	else
		ttl_ = Random::floatRange(48.0f, 51.0f);

	hunger_ = cells_.size() + 2.0f;

	for (const Cell* cell : cells_)
		rigidBody_->setMass(rigidBody_->getMass() + cell->getMass());

	rigidBody_->setPosition(position);

	for (auto* cell : cells_)
		cell->init();	

	if (isMover_)
		speedMove_ = (numMovers_ * 75.0f + (numMovers_ - 1) * 80.0f) / cells_.size();

	if (isPlant_)
		rigidBody_->setLinearFriction(0.9f);
}

Organism::Organism(DNA dna, Vec2f position, float angle) :
	Organism(dna, getCellsForDNA(dna.get()), position, angle)
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
	age_ += Time::DeltaSeconds;

	if (!isCorpse_ && age_ >= ttl_)
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

			if (cells_.size() == 1 && cells_[0]->getType() == Type::Thorn)
				markForDeath();
		}
		else
			it++;
	}

	if (cells_.size() == 0)
	{
		wantsToDie_ = true;
		aabb_->wantsToBeDeleted = true;
	}

	if (isCorpse_)
		return;

	for (Cell* cell : cells_)
		cell->tick();	

	if (isMover_)
	{
		rigidBody_->addTorque(Random::floatRange(-torqueFactor_, torqueFactor_));
		float s, c;
		auto rot = rigidBody_->getRotation();
		s = sin(rot);
		c = cos(rot);
		Vec2f forward = { -s, c };
		rigidBody_->addImpulse(forward * 5000.0f);

		if (rigidBody_->getVelocity().magnitude() > speedMove_)
			rigidBody_->setVelocity(rigidBody_->getVelocity().normalized() * speedMove_);
	}
}

Organism* Organism::clone(Vec2f pos)
{
	DNA successor = DNA(dna_.get());

	if (isPlant_)
	{
		if (Random::unsignedRange(0, OneInNMutates * 2) == 0)
			successor.mutate();
	}
	else	
	{
		if (Random::unsignedRange(0, OneInNMutates) == 0)
			successor.mutate();
	}

	auto cells = getCellsForDNA(successor.get());
	return cells.size() > 0 ? new Organism(successor, cells, pos, Random::floatRange(0, 2 * std::numbers::pi)) : nullptr;
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

	Organism* ret = new Organism(dna_, corpseCells, rigidBody_->getPosition(), rigidBody_->getRotation());
	ret->setCorpse(true);
	ret->setMover(isMover_);
	ret->isPlant_ = isPlant_;
	ret->isThorn_ = isThorn_;
	ret->isHerbivore_ = isHerbivore_;
	return ret;
}

AABB* Organism::getAABB()
{
	return aabb_;
}

float Organism::getReproductionUrge()
{
	return energy_ / hunger_;
}

float Organism::getAge()
{
	return age_;
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

bool Organism::isHerbivore() const
{
	return isHerbivore_;
}

bool Organism::isPlant() const
{
	return isPlant_;
}

bool Organism::isThorn() const
{
	return isThorn_;
}

bool Organism::isCarnivore() const
{
	return isCarnivore_;
}

unsigned Organism::getSize()
{
	return cells_.size();
}

void Organism::setEnergy(float energy)
{
	energy_ = energy;
}

float Organism::getEnergy()
{
	return energy_;
}

float Organism::getHunger()
{
	return hunger_;
}

RigidBody* Organism::getRigidBody()
{
	return rigidBody_;
}

void Organism::addMover()
{
	numMovers_++;
}

void Organism::removeMover()
{
	numMovers_--;
}

DNA& Organism::getDNA()
{
	return dna_;
}

bool Organism::isCorpse()
{
	return isCorpse_;
}

void Organism::setCorpse(bool corpse)
{
	isCorpse_ = corpse;
	aabb_->isCorpse = true;
}

void Organism::setMover(bool mover)
{
	isMover_ = mover;
	aabb_->isMover = mover;
}

long Organism::getTTL()
{
	return ttl_;
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
		return new HerbivoreCell();
	case 'C':
		return new CarnivoreCell();
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
	std::string cells = "PTMOCA";

	for (size_t index = 0; index < dna.size(); index++)
	{
		const char instruction = dna[index];

		if (cells.find(instruction) != std::string::npos)
		{
			Vec2f offset = { sin(curRot), cos(curRot) };
			curPos += offset * Cell::Size;
			bool found = false;

			for (Cell* cell : ret)
				if ((cell->getLocalPos() - curPos).magnitude() < Cell::Size * 0.5f)
					found = true;

			if (!found)
			{
				Cell* cell = getCellForSymbol(instruction);
				cell->setLocalPos(curPos);
				ret.push_back(cell);
			}
		}
		else if (instruction == 'L')
		{
			curRot -= 0.5f * std::numbers::pi;
		}
		else if (instruction == 'R')
		{
			curRot += 0.5f * std::numbers::pi;
		}
		else if (instruction == '[')
		{
			savedPositions.push_back(curPos);
			savedRotations.push_back(curRot);
			stackIdx++;
		}
		else if (instruction == ']')
		{
			curPos = savedPositions[stackIdx];
			curRot = savedRotations[stackIdx];
			savedPositions.pop_back();
			savedRotations.pop_back();
			stackIdx--;
		}
	}

	return ret;
}