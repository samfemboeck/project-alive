#pragma once
#include "Cell.h"
#include "Engine/Timer.h"

class Organism;
struct CircleCollider;

class LeafCell final : public Cell
{
public:
	LeafCell();
	void tick() override;
	float getNutritionValue() const;
	CorpseCell* createCorpse() const override;
	float getMass() const override;

private:
	float elapsed_ = 0;
	bool isLifetimeExpired_ = false;
};