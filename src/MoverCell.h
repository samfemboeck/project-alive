#pragma once

#include "Cell.h"

class MoverCell final : public Cell
{
public:
	MoverCell();
	void init() override;
	void tick() override;
	CorpseCell* createCorpse() const override;
	void setSpeedMove(float speed);
	void setSpeedTurn(float speed);

private:
	void findNewTarget();

private:
	float speedTurn_ = 10.0f;
	float speedMove_ = 100.0f;
	Vec2f target_;
	unsigned numFoodEaten = 0;
	float ttl_;
	float energy_ = 0.0f;
};