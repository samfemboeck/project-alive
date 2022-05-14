#include "../pch.h"
#include "QuickMaths.h"

static std::random_device rd; // obtain a random number from hardware
static std::mt19937 gen(rd()); // seed the generator

Vec2 Random<Vec2>::range(const Vec2& min, const Vec2& max)
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

Vec2::Vec2(float x, float y)
{
	this->x = x;
	this->y = y;
}

Vec2::Vec2(const b2Vec2& vec)
{
	x = vec.x;
	y = vec.y;
}

Vec2::Vec2(const glm::vec2& vec)
{
	x = vec.x;
	y = vec.y;
}

Vec2::operator const glm::vec2() const
{ 
	return glm::vec2(x, y);
}

Vec2::operator const b2Vec2() const
{ 
	return b2Vec2(x, y);
}

std::string Vec2::str() const
{
	return "{" + std::to_string(x) + ", " + std::to_string(y) + "}";
}

Vec2& Vec2::operator+=(const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator*(float rhs) const
{
	return Vec2(x * rhs, y * rhs);
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

float Vec2::magnitude()
{
	return sqrt(x * x + y * y);
}

float Vec2::magnitude_squared()
{
	return x * x + y * y;
}

Vec2 Vec2::normalize()
{
	float mag = magnitude();
	return Vec2(x / mag, y / mag);
}

float Vec2::dot(const Vec2& other)
{
	return x * other.x + y * other.y;
}

float Vec2::cross(const Vec2& other) const
{
	return (x * other.y) - (y * other.x);
}
