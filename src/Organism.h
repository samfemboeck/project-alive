#pragma once

#include "Engine/QuickMaths.h"
#include "Engine/Timer.h"

class RigidBody;
class Cell;
class AABB;

class Organism
{
public:
	inline static long MaxTTL = 10000;
	inline static long Instances = 0;
	inline static std::vector<std::string> DefaultDNAs = {
		"L(0)",
		"M(0)",
		"A(0)", 
		"X(0)",
		"L(0)T(0)R(0)",
		"L(1)R(3)R(0)",
	};
	inline static unsigned DNAIndex = 0;

public:
	Organism(const std::string& dna, Vec2f position_, float angle);
	~Organism();
	void draw() const;
	void tick();
	Organism* clone(Vec2f pos);
	Organism* createCorpse();
	AABB* getAABB();
	void setPosition(Vec2f pos);
	bool wantsToBeDeleted();
	bool wantsToDie();
	bool isMover();
	bool isLeaf();
	bool isCorpse();
	unsigned getReproductionCount();
	void setReproductionCount(unsigned count);
	long getAgeMs();
	void markForDeletion();
	void markForDeath();

	RigidBody* getRigidBody();
	std::string dna_;

private:
	std::vector<Cell*> cells_;
	std::chrono::high_resolution_clock::time_point start_;
	RigidBody* rigidBody_;
	AABB* aabb_;
	bool isCorpse_ = false;
	bool isLeaf_ = false;
	bool isMover_ = false;
	bool wantsToBeDeleted_ = false;
	bool wantsToDie_ = false;
	unsigned reproductionCount_ = 0;
	Vec2f offsetCenterToRb_;
};
