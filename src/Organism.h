#pragma once

#include "Engine/QuickMaths.h"
#include "Engine/Timer.h"
#include "Engine/Physics.h"
#include "Mutation.h"

class RigidBody;
class Cell;
class AABB;

class Organism
{
public:
	inline static long MaxTTL = 10000;
	inline static long Instances = 0;
	inline static int OneInNMutates = 40;
	
public:
	static Cell* getCellForSymbol(char symbol);
	static Vec2f getOffsetForParam(unsigned param);
	static std::vector<Cell*> getCellsForDNA(std::string dna);

public:
	Organism(DNA dna, const std::vector<Cell*>& cells, Vec2f position, float angle);
	Organism(DNA dna, Vec2f position, float angle);
	~Organism();
	void draw() const;
	void tick();
	Organism* clone(Vec2f pos);
	Organism* createCorpse();
	AABB* getAABB();
	bool wantsToDie();
	float getReproductionUrge();
	float getAge();
	void markForDeath();
	float getMass();
	void removeCell(Cell* cell);
	bool isMover() const;
	bool isMouth() const;
	bool isPlant() const;
	bool isPredator() const;
	unsigned getSize();
	void setEnergy(float energy);
	float getEnergy();
	float getHunger();
	RigidBody* getRigidBody();
	void addMover();
	void removeMover();
	DNA& getDNA();
	bool isCorpse();
	void setCorpse(bool corpse);
	void setMover(bool mover);
	long getTTL();

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
	DNA dna_;
	std::vector<Cell*> cells_;
	std::chrono::high_resolution_clock::time_point start_;
	RigidBody* rigidBody_ = nullptr;
	AABB* aabb_ = nullptr;
	bool wantsToDie_ = false;
	bool isMover_ = false;
	bool isCorpse_ = false;
	bool isPlant_ = false;
	bool isPredator_ = false;
	bool isMouth_ = false;
	std::vector<Cell*> toRemove_;
	float energy_ = 0.0f;
	unsigned numMovers_ = 0;
	float torqueFactor_ = 100.0f;
	float age_ = 0.0f;

	float ttl_;	
	float speedMove_;
	float hunger_;
};
