#pragma once
#include "Engine/QuickMaths.h"

class Organism;
class CircleCollider;

struct Cell
{
	inline static int Instances = 0;
	inline static const float Size = 10;

	std::string textureName;
	Vec2 localPos;

	Cell(std::string textureName);
	void draw();
	virtual void tick(Organism* org) {};
	inline virtual ~Cell() { Instances--; };
	virtual void onCollision(CircleCollider* coll) {}
};