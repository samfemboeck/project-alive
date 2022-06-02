#pragma once

#include "Engine/QuickMaths.h"
#include "Engine/Timer.h"
#include "Engine/Physics.h"

class RigidBody;
class Cell;
class AABB;

class Organism
{
public:
	inline static long MaxTTL = 10000;
	inline static long Instances = 0;
	
public:
	static Cell* getCellForSymbol(char symbol);
	static Vec2f getOffsetForParam(unsigned param);
	static std::vector<Cell*> getCellsForDNA(std::string dna);

public:
	Organism(std::string dna, const std::vector<Cell*>& cells, Vec2f position, float angle);
	Organism(std::string dna, Vec2f position, float angle);
	~Organism();
	void draw() const;
	void tick();
	Organism* clone(Vec2f pos);
	Organism* createCorpse();
	AABB* getAABB();
	bool wantsToDie();
	unsigned getReproductionUrge();
	void setReproductionUrge(unsigned count);
	long getAgeMs();
	void markForDeath();
	float getMass();
	void removeCell(Cell* cell);
	bool isMover() const;
	unsigned getSize();
	void addEnergy(float energy);
	RigidBody* getRigidBody();

	template<typename T>
	T* getCell()
	{
		for (Cell* cell : cells_)
		{
			T* t = dynamic_cast<T*>(cell);
			if (t)
				return t;
		}

		return nullptr;
	}

private:
	std::string dna_;
	std::vector<Cell*> cells_;
	std::chrono::high_resolution_clock::time_point start_;
	RigidBody* rigidBody_;
	AABB* aabb_;
	bool wantsToDie_ = false;
	unsigned reproductionCount_ = 0;
	bool isMover_ = false;
	bool isCorpse_ = false;
	std::vector<Cell*> toRemove_;
	long ttl_;	
	float energy_ = 0.0f;
	float hunger_;
};
