#pragma once

#include "Cell.h"

class CorpseCell final : public Cell
{
public:
	CorpseCell(float nutritionValue, Vec2f localPos, bool isPlant);
	float getNutritionValue() const;
	void init() override;
	char getSymbol() override;
	bool isPlant() const;

private:
	float nutritionValue_ = 50.0f;
	bool isPlant_;
};