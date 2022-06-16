#include "Cell.h"

class MouthCell : public Cell
{
public:
	MouthCell();
	void onCollision(Cell* other) override;
	CorpseCell* createCorpse() const override;
	void init() override;
	float getMass() const override;

private:
	float energy_ = 0.0f;
	float hunger_;
};