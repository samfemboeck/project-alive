#include "pch.h"
#include "FruitCell.h"
#include "Organism.h"

FruitCell::FruitCell(Organism* org, CircleCollider* collider) :
	Cell(org, collider, "cell_fruit")
{
}
