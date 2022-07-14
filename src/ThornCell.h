#pragma once

#include "Cell.h"

class ThornCell final : public Cell
{
public:
	ThornCell();
	void onCollision(Cell* other);
	CorpseCell* createCorpse() const override;
	char getSymbol() override;
	float getMass() const override;
};