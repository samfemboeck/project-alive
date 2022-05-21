#pragma once
#include <unordered_map>
#include <array>
#include "QuickMaths.h"

struct AABB;

class SpatialHash
{
public:
	inline static const unsigned GridWidth = 140;
	inline static const unsigned GridHeight = 140;
	inline static const unsigned MaxEntitiesPerSquare = 80u;
	inline static const unsigned SquareSize = 80u;

public:
	SpatialHash();
	bool add(AABB* aabb);
	void clear();
	void setPos(Vec2f pos);
	void draw();
	Vec2i getCoord(Vec2f worldPos);
	std::array<AABB*, MaxEntitiesPerSquare>& get(unsigned x, unsigned y);

private:
	Vec2f pos_ = Vec2f(0, 0);
	std::array<std::array<std::array<AABB*, MaxEntitiesPerSquare>, GridWidth>, GridHeight> grid_;
};