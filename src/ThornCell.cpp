#include "pch.h"
#include "ThornCell.h"
#include "Organism.h"
#include "Engine/Physics.h"

ThornCell::ThornCell(Organism* org, CircleCollider* collider) :
	Cell(org, collider, "cell_thorn")
{
}
