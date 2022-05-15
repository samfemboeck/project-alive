#pragma once
#include "Cell.h"

class Organism;
class CircleCollider;

class LeafCell final : public Cell
{
public:
	LeafCell(Organism*, CircleCollider*);
	void tick() override;
	void onCollision(Cell*) override;
};