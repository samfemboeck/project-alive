#include "../pch.h"
#include "QuickMaths.h"

static std::random_device rd; // obtain a random number from hardware
static std::mt19937 gen(rd()); // seed the generator

Vec2f Random<Vec2f>::range(const Vec2f& min, const Vec2f& max)
{
	std::uniform_real_distribution<float> dX(min.x, max.x); // define the range
	std::uniform_real_distribution<float> dY(min.y, max.y); // define the range
	return { dX(gen), dY(gen) };
}

float Random<float>::range(const float& min, const float& max)
{
	std::uniform_real_distribution<float> d(min, max); // define the range
	return d(gen);
}

int Random<int>::range(const int& min, const int& max)
{
	std::uniform_int_distribution d(min, max); // define the range
	return d(gen);
}