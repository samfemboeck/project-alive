module;
#include <box2d/b2_math.h>
#include <glm/glm.hpp>
#include <string>
#include <random>
export module QuickMaths;

export struct Vec2
{	
	Vec2() = default;

	Vec2(float x, float y)
	{
		X = x;
		Y = y;
	}

	Vec2(const b2Vec2& vec)
	{
		X = vec.x;
		Y = vec.y;
	}

	Vec2(const glm::vec2& vec)
	{
		X = vec.x;
		Y = vec.y;
	}

	operator const glm::vec2() const { return glm::vec2(X, Y); }

	operator const b2Vec2() const { return b2Vec2(X, Y); }

	std::string str() const { return "{" + std::to_string(X) + ", " + std::to_string(Y) + "}"; }

	Vec2& operator += (const Vec2& rhs)
	{
		X += rhs.X;
		Y += rhs.Y;
		return *this;
	}

	Vec2 operator+(const Vec2& rhs) const
	{
		return Vec2(X + rhs.X, Y + rhs.Y);
	}		
	
	Vec2 operator*(float rhs) const
	{
		return Vec2(X * rhs, Y * rhs);
	}
	
	Vec2 operator-(const Vec2& rhs) const
	{
		return Vec2(X - rhs.X, Y - rhs.Y);
	}

	float magnitude()
	{
		return sqrt(X * X + Y * Y);
	}

	Vec2 normalize()
	{
		float mag = magnitude();
		return Vec2(X / mag, Y / mag);
	}

	float X, Y;
};

static std::random_device rd; // obtain a random number from hardware
static std::mt19937 gen(rd()); // seed the generator

export struct Random
{
	static float float_range(float min, float max)
	{
		std::uniform_real_distribution<float> d(min, max); // define the range
		return d(gen);
	}

	static Vec2 vec2_range(const Vec2& min, const Vec2& max)
	{
		std::uniform_real_distribution<float> d_x(min.X, max.X); // define the range
		std::uniform_real_distribution<float> d_y(min.Y, max.Y); // define the range
		return { d_x(gen), d_y(gen) };
	}

	static int int_range(const int& min, const int& max)
	{
		std::uniform_int_distribution<int> d(min, max); // define the range
		return d(gen);
	}
};