#pragma once
#include "Cell.h"
#include "Engine/Timer.h"

class Organism;
struct CircleCollider;

class LeafCell final : public Cell
{
public:
	LeafCell(Organism*, CircleCollider*);
	void tick() override;
	void onCollision(Cell*) override;
	void setLit(bool);

private:
	bool isLit_ = false;
};