module;
#include <string>
export module Cell;

import QuickMaths;

export struct Cell
{
	Cell(std::string texture_name) : 
		TextureName(texture_name)
	{}

	virtual ~Cell() = default;
	static const float Size;
	std::string TextureName;
	Vec2 LocalPos;
};

export const float Cell::Size = 10;

export class LeafCell final : public Cell
{
public:
	LeafCell() : Cell("cell_leaf.png"){};
};

export class MoverCell final : public Cell
{
public:
	MoverCell() : Cell("cell_mover.png"){};
};

export class ThornCell final : public Cell
{
public:
	ThornCell() : Cell("cell_mover.png"){};
};
