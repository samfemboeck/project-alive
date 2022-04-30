#pragma once

struct Vec2
{	
	inline Vec2() = default;

	inline Vec2(float x, float y)
	{
		X = x;
		Y = y;
	}

	inline Vec2(const b2Vec2& vec)
	{
		X = vec.x;
		Y = vec.y;
	}

	inline Vec2(const glm::vec2& vec)
	{
		X = vec.x;
		Y = vec.y;
	}

	inline operator const glm::vec2() const { return glm::vec2(X, Y); }

	inline operator const b2Vec2() const { return b2Vec2(X, Y); }

	inline Vec2& operator += (const Vec2& rhs)
	{
		X += rhs.X;
		Y += rhs.Y;
		return *this;
	}

	Vec2 operator+(const Vec2& rhs) const
	{
		return Vec2(X + rhs.X, Y + rhs.Y);
	}	
	
	Vec2 operator-(const Vec2& rhs) const
	{
		return Vec2(X - rhs.X, Y - rhs.Y);
	}

	float X, Y;
};

static std::random_device rd; // obtain a random number from hardware
static std::mt19937 gen(rd()); // seed the generator

class QuickMaths
{
public:
	inline static float mag(const Vec2& vec)
	{
		return sqrt(vec.X * vec.X + vec.Y * vec.Y);
	}

	inline static Vec2 rand_vec(const Vec2& min, const Vec2& max)
	{
		std::uniform_real_distribution<float> d_x(min.X, max.X); // define the range
		std::uniform_real_distribution<float> d_y(min.Y, max.Y); // define the range
		return {d_x(gen), d_y(gen)};
	}
};
