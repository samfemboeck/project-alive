#pragma once
#include "Engine/QuickMaths.h"

class Organism;
struct CircleCollider;

class Cell
{
public:
	inline static int Instances = 0;
	inline static const float Size = 40;

public:
	Cell(Organism* organism, CircleCollider* collider, std::string textureName);
	virtual void tick() {};
	virtual ~Cell();
	virtual void onCollision(Cell* other) {}
	virtual void draw();
	virtual void init() {};
	Organism* organism_;

protected:
	friend class Organism;
	CircleCollider* collider_;
	std::string textureName_;
	std::string textureNameFilled_;
};