#pragma once

#include "Engine/QuickMaths.h"
#include "Engine/Timer.h"

class RigidBody;
class Cell;
class AABB;

class Organism
{
public:
	inline static int MaxInstances = 2000;
	inline static long MaxTTL = 10000;
	inline static long ActiveInstances = 0;

	inline static std::vector<std::string> DefaultDNAs = {
		"L(0)",
		"M(0)",
		"A(0)", 
		"X(0)",
		"L(0)T(0)R(0)",
		"L(1)R(3)R(0)",
	};

	inline static unsigned DNAIndex = 0;
	inline static std::vector<Organism*> Instances;

public:
	Organism(const std::string& dna, std::function<float(float)> instinct, Vec2f position_, float angle);
	~Organism();
	void draw() const;
	int tick();
	Organism* clone(Vec2f pos);
	Organism* createCorpse();
	AABB* getAABB();
	void setPosition(Vec2f pos);
	void onCollision(Cell*,Cell*);
	bool isDeleted();
	bool isMover();
	bool isLeaf();
	std::string dna_;

private:
	friend class LeafCell;
	friend class MoverCell;
	friend class LightCell;
	
private:
	std::vector<Cell*> cells_;
	std::function<float(float)> instinct_;
	long initialTTL_;
	TickCountTimer timerTTL_;
	RigidBody* rigidBody_;
	AABB* aabb_;
	bool isLight_ = false;
	float energy_ = 100.0f;
	bool isCorpse_ = false;
	bool isLeaf_ = false;
	bool isMover_ = false;
	bool isDeleted_ = false;
	Vec2f offsetCenterToRb_;
};
