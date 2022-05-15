#include "pch.h"
#include "LeafCell.h"
#include "Organism.h"
#include "Engine/Physics.h"

LeafCell::LeafCell(Organism* org, CircleCollider* collider) : 
	Cell(org, collider, "cell_leaf.png")
{
}

void LeafCell::tick()
{
}

void LeafCell::onCollision(Cell* other)
{
}
