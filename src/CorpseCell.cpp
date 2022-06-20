#include "pch.h"
#include "CorpseCell.h"
#include "Organism.h"

CorpseCell::CorpseCell(float nutritionValue, Vec2f localPos) :
	Cell("cell_food"),
	nutritionValue_(nutritionValue)
{
	type_ = Type::Corpse;
	localPos_ = localPos;
}

float CorpseCell::getNutritionValue() const
{
	return nutritionValue_;
}

void CorpseCell::init()
{
	collider_->isSensor = true;
}
