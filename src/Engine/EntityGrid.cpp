#include "EntityGrid.h"
#include "Physics.h"
#include <iostream>
#include "../pch.h"
#include "../Cell.h"
#include "Renderer2D.h"

SpatialHash::SpatialHash()
{
	AABB* a1 = new AABB();
	a1->boundsWorld.min = { 0, 0 };
	a1->boundsWorld.max = { Cell::Size, Cell::Size };
	add(a1);
	clear();
	AABB* a2 = new AABB();
	a1->boundsWorld.min = { 0, 0 };
	a1->boundsWorld.max = { Cell::Size, Cell::Size };
	add(a2);
}

bool SpatialHash::add(AABB* aabb)
{
	Bounds& bounds = aabb->boundsWorld;
	int startX = (bounds.min.x - pos_.x) / SquareSize;
	int startY = (bounds.min.y - pos_.y) / SquareSize;
	int endX = (bounds.max.x - pos_.x) / SquareSize;
	int endY = (bounds.max.y - pos_.y) / SquareSize;

	if 
	(
		startX < 0 ||
		startY < 0 ||
		endX >= GridWidth ||
		endY >= GridHeight
	)
	{
		LOG("Out of board bounds");
		return false;
	}

	aabb->regStart = {startX, startY};
	aabb->regEnd = {endX, endY};

	for (unsigned x = startX; x <= endX; x++)
	{
		for (unsigned y = startY; y <= endY; y++)
		{
			for (unsigned i = 0; i < MaxEntitiesPerSquare; i++)
			{
				if (grid_[x][y][i] == nullptr)
				{
					grid_[x][y][i] = aabb;
					break;
				}
				
				if (i == MaxEntitiesPerSquare - 1)
					throw std::exception("Square Overflow");
			}
		}
	}

	return true;
}

void SpatialHash::clear()
{
	memset(grid_.data(), 0, GridWidth * GridHeight * MaxEntitiesPerSquare * sizeof(AABB*));
}

void SpatialHash::setPos(Vec2f pos)
{
	pos_ = pos;
}

void SpatialHash::draw()
{
	for (unsigned x = 0; x < GridWidth; x++)
	{
		Vec2i posMin = Vec2i(pos_.x + x * SquareSize, pos_.y);
		Vec2i posMax = Vec2i(posMin.x + SquareSize, posMin.y + GridHeight * SquareSize);
		Renderer2D::pushQuad(posMin, posMax, TextureManager::get("aabb.png"));
	}
	for (unsigned y = 0; y < GridHeight; y++)
	{

		Vec2i posMin = Vec2i(pos_.x, pos_.y + y * SquareSize);
		Vec2i posMax = Vec2i(posMin.x + GridWidth * SquareSize, posMin.y + SquareSize);
		Renderer2D::pushQuad(posMin, posMax, TextureManager::get("aabb.png"));
	}
}

Vec2i SpatialHash::getCoord(Vec2f worldPos)
{
	int x = (worldPos.x - pos_.x) / SquareSize;
	int y = (worldPos.y - pos_.y) / SquareSize;
	return { x, y };
}

std::array<AABB*, SpatialHash::MaxEntitiesPerSquare>& SpatialHash::get(unsigned x, unsigned y)
{
	return grid_[x][y];
}
