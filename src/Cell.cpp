#include "pch.h"
#include "Cell.h"

const float Cell::Size = 10;

Cell::Cell(std::string texture_name) :
	TextureName(texture_name)
{
}

Cell::~Cell()
{
}

void Cell::draw()
{	
}
