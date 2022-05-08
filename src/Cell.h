#pragma once
#include "Engine/QuickMaths.h"

class Organism;

struct Cell
{
	static const float Size;
	std::string TextureName;
	Vec2 LocalPos;

	Cell(std::string texture_name);
	void draw();
	virtual void tick(Organism* org) {};
	virtual ~Cell() = default;
};