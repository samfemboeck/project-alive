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
	long getTtl();

private:
	float elapsed_ = 0;
	long ttl_;
	bool isLifetimeExpired_ = false;
};