#include "pch.h"
#include "Cell.h"

Cell::Cell(std::string textureName) :
	textureName(textureName)
{
	Instances++;
}