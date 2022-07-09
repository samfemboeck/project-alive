#include "pch.h"
#include "CorpseCell.h"
#include "Organism.h"

CorpseCell::CorpseCell(float nutritionValue, Vec2f localPos, bool isPlant) :
	Cell("cell_corpse.png"),
	nutritionValue_(nutritionValue),
	isPlant_(isPlant)
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

char CorpseCell::getSymbol()
{
	return 'X';
}

bool CorpseCell::isPlant() const
{
	return isPlant_;
}
