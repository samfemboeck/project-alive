#pragma once

#include "Cell.h"

class Organism;
struct CircleCollider;

class FruitCell final : public Cell
{
public:
	FruitCell(Organism*, CircleCollider*);
};
