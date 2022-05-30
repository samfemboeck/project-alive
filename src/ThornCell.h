#pragma once

#include "Cell.h"

class ThornCell final : public Cell
{
public:
	ThornCell();
	void onCollision(Cell* other);
	CorpseCell* createCorpse() const override;
	void init() override;
	void tick() override;
	long ttl_;
};