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

	inline std::string str() const { return "{" + std::to_string(X) + ", " + std::to_string(Y) + "}"; }

	inline Vec2& operator += (const Vec2& rhs)
	{
		X += rhs.X;
		Y += rhs.Y;
		return *this;
	}

	inline Vec2 operator+(const Vec2& rhs) const
	{
		return Vec2(X + rhs.X, Y + rhs.Y);
	}		
	
	inline Vec2 operator*(float rhs) const
	{
		return Vec2(X * rhs, Y * rhs);
	}
	
	inline Vec2 operator-(const Vec2& rhs) const
	{
		return Vec2(X - rhs.X, Y - rhs.Y);
	}

	inline float magnitude()
	{
		return sqrt(X * X + Y * Y);
	}

	inline Vec2 normalize()
	{
		float mag = magnitude();
		return Vec2(X / mag, Y / mag);
	}

	float X, Y;
};

template<typename T>
struct Random
{
	static T range(const T& min, const T& max);
};