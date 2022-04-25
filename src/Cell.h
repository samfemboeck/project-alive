#pragma once
#include "Engine/Transform.h"

class Cell
{
public:
	static const float s_size;
	static int s_instances;
public:
	Cell(std::string texture_name);
	void draw();
	virtual ~Cell();
protected:
	friend class Organism;
	std::string m_texture_name;
	Transform m_transform;
	int m_id;
};