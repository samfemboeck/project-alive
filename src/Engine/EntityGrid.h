#pragma once
#include <unordered_map>
#include <array>
#include "QuickMaths.h"
#include "../Cell.h"

struct AABB;

class EntityGrid
{
public:
	inline static const unsigned GridWidth = 280;
	inline static const unsigned GridHeight = 280;
	inline static const unsigned MaxEntitiesPerSquare = 50u;
	inline static const unsigned SquareSize = Cell::Size * 2;

public:
	EntityGrid();
	bool add(AABB* aabb);
	void clear();
	void draw();
	Vec2i getLocalCoord(Vec2f worldPos);
	Vec2f getWorldPos(Vec2i localCoord);
	std::array<AABB*, MaxEntitiesPerSquare>& get(unsigned x, unsigned y);

private:
	Vec2f pos_ = Vec2f(0, 0);
	std::array<std::array<std::array<AABB*, MaxEntitiesPerSquare>, GridWidth>, GridHeight> grid_;
};