#pragma once

#include "Cell.h"

struct LightCell final : public Cell
{
	inline static float Radius = Cell::Size * 100;
	LightCell();
};
