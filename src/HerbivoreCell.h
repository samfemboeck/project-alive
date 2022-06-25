#include "Cell.h"

class HerbivoreCell : public Cell
{
public:
	HerbivoreCell();
	void onCollision(Cell* other) override;
	CorpseCell* createCorpse() const override;
	void init() override;
	float getMass() const override;
	char getSymbol() override;
};