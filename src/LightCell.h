#pragma once

#include "Cell.h"

class LightCell final : public Cell
{
public:
	inline static float LightRadius = Cell::Size * 20;
	LightCell(Organism* org, CircleCollider* collider);
};
