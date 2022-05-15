#pragma once

#include "Engine/QuickMaths.h"
#include "Engine/Timer.h"

class RigidBody;
class Cell;

struct Organism
{
public:
	inline static int MaxInstances = 2000;
	inline static long TimeToLive = 2000;
	inline static long ActiveInstances = 0;

	inline static std::vector<std::string> DefaultDNAs = {
		"L(0)",
		"M(0)L(0)T(3)T(3)T(3)",
		"A(0)", 
		"L(0)T(0)",
	};

	inline static unsigned DNAIndex = 0;
	inline static std::vector<Organism*> Instances;

public:
	Organism(const std::string& dna, std::function<float(float)> instinct, Vec2 position_, float angle);
	~Organism();
	void draw() const;
	int tick();
	Organism* clone();

private:
	friend class LeafCell;
	friend class MoverCell;
	
private:
	std::string dna_;
	std::vector<Cell*> cells_;
	std::function<float(float)> instinct_;
	long initialTTL_;
	TickCountTimer timerTTL_;
	RigidBody* rigidBody_;
};
