#pragma once

#include "Cell.h"

class MoverCell final : public Cell
{
public:
	MoverCell();
	~MoverCell() override;
	void init() override;
	void tick() override;
	CorpseCell* createCorpse() const override;
	float getMass() const override;

private:
	float speedTurn_ = 10.0f;
	float speedMove_ = 100.0f;
	float ttl_;
	float energy_ = 0.0f;
};