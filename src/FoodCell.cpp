#include "pch.h"
#include "FoodCell.h"
#include "Organism.h"

FoodCell::FoodCell(Organism* org, CircleCollider* collider) :
	Cell(org, collider, "cell_food")
{
}
