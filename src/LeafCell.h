#pragma once
#include "Cell.h"

class LeafCell final : public Cell
{
public:
	LeafCell() : Cell("cell_leaf.png"){};
};