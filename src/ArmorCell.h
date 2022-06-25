#pragma once

#include "Cell.h"

class ArmorCell final : public Cell
{
public:
	ArmorCell();
	void onCollision(Cell* other);
	CorpseCell* createCorpse() const override;
	char getSymbol() override;
};