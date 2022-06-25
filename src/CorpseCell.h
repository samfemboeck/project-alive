#pragma once

#include "Cell.h"

class CorpseCell final : public Cell
{
public:
	CorpseCell(float nutritionValue, Vec2f localPos);
	float getNutritionValue() const;
	void init() override;
	char getSymbol() override;

private:
	float nutritionValue_ = 50.0f;
};