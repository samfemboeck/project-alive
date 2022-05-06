#pragma once

struct LightCell final : public Cell
{
	float Radius = Cell::Size * 6;
	LightCell() : Cell("cell_light.png"){};
};
