#pragma once
#include "Engine/QuickMaths.h"

class Organism;
class CircleCollider;

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

protected:
	CircleCollider* collider_;
	std::string textureName_;
	std::string textureNameFilled_;
	Organism* organism_;
};