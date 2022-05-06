#include "../pch.h"
#include "QuickMaths.h"

static std::random_device rd; // obtain a random number from hardware
static std::mt19937 gen(rd()); // seed the generator

Vec2 Random<Vec2>::range(const Vec2& min, const Vec2& max)
{
	std::uniform_real_distribution<float> d_x(min.X, max.X); // define the range
	std::uniform_real_distribution<float> d_y(min.Y, max.Y); // define the range
	return { d_x(gen), d_y(gen) };
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
