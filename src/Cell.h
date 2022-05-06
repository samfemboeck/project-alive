#pragma once
#include "Engine/QuickMaths.h"

struct Cell
{
	static const float Size;
	std::string TextureName;
	Vec2 LocalPos;

	Cell(std::string texture_name);
	void draw();
	virtual ~Cell();
};