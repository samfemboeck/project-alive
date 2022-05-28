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
private:
	float elapsed_ = 0;
	long ttl_;
};