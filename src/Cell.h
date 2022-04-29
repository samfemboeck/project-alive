#pragma once

struct Cell
{
	static const float Size;
	std::string TextureName;
	glm::vec2 LocalPos;

	Cell(std::string texture_name);
	void draw();
	virtual ~Cell();
};