#pragma once

#include "Cell.h"
#include <vector>

struct AABB;

class LightCell final : public Cell
{
public:
	inline static float LightRadius = Cell::Size * 80;
	LightCell(Organism* org, CircleCollider* collider);
	void draw() override;
	void tick() override;
private:
	std::vector<AABB*> queryResult_;
};
