#pragma once

#include "Cell.h"

class MoverCell final : public Cell
{
public:
	MoverCell();
	~MoverCell() override;
	void init() override;
	CorpseCell* createCorpse() const override;
	float getMass() const override;
	char getSymbol() override;
};