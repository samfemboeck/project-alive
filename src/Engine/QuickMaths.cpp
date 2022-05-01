#include "../pch.h"
#include "QuickMaths.h"

Vec2 Random<Vec2>::range(const Vec2& min, const Vec2& max)
{
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_real_distribution<float> d_x(min.X, max.X); // define the range
	std::uniform_real_distribution<float> d_y(min.Y, max.Y); // define the range
	return { d_x(gen), d_y(gen) };
}

float Random<float>::range(const float& min, const float& max)
{
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_real_distribution<float> d(min, max); // define the range
	return d(gen);
}
