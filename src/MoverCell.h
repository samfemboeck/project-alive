#pragma once

#include "Cell.h"

class MoverCell final : public Cell
{
public:
	MoverCell(Organism* org, CircleCollider* collider);
	void init() override;
	void tick() override;

private:
	void findNewTarget();

private:
	float speedTurn_ = 10.0f;
	float speedMove_ = 70.0f;
	Vec2f target_;
};