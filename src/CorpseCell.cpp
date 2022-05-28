#include "pch.h"
#include "CorpseCell.h"
#include "Organism.h"

CorpseCell::CorpseCell(Organism* org, CircleCollider* collider) :
	Cell(org, collider, "cell_food")
{
}

void CorpseCell::tick()
{
}
