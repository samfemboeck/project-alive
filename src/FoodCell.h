#pragma once

#include "Cell.h"

class Organism;
struct CircleCollider;

class FoodCell final : public Cell
{
public:
	FoodCell(Organism*, CircleCollider*);
};