#include "pch.h"
#include "LightCell.h"
#include "Organism.h"
#include "Engine/Physics.h"

LightCell::LightCell(Organism* org, CircleCollider* collider) :
	Cell(org, collider, "cell_light.png")
{
}
