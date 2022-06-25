#pragma once

#include "Cell.h"

class CarnivoreCell final : public Cell
{
public:
	CarnivoreCell();
	void onCollision(Cell* other);
	CorpseCell* createCorpse() const override;
	char getSymbol() override;
};