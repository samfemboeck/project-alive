#pragma once

#include <glm/glm.hpp>
#include <string>
#include <box2d/b2_math.h>

template<typename T>
struct Vec2
{	
	Vec2() = default;

	Vec2(T x, T y)
	{
		this->x = x;
		this->y = y;
	}

	Vec2(const b2Vec2& vec)
	{
		x = vec.x;
		y = vec.y;
	}

	Vec2(const glm::vec2& vec)
	{
		x = vec.x;
		y = vec.y;
	}

	operator const glm::vec2() const
	{
		return glm::vec2(x, y);
	}

	operator const b2Vec2() const
	{
		return b2Vec2(x, y);
	}

	std::string str() const
	{
		return "{" + std::to_string(x) + ", " + std::to_string(y) + "}";
	}

	Vec2<T>& operator+=(const Vec2<T>& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vec2<T> operator+(const Vec2<T>& rhs) const
	{
		return Vec2<T>(x + rhs.x, y + rhs.y);
	}

	Vec2<T> operator*(float rhs) const
	{
		return Vec2<T>(x * rhs, y * rhs);
	}

	Vec2<T> operator-(const Vec2<T>& rhs) const
	{
		return Vec2<T>(x - rhs.x, y - rhs.y);
	}	
	
	Vec2<T> operator-(const float& f) const
	{
		return Vec2<T>(x - f, y - f);
	}

	Vec2<T> operator+(const float& f) const
	{
		return Vec2<T>(x + f, y + f);
	}

	T magnitude()
	{
		return sqrt(x * x + y * y);
	}

	T magnitude_squared()
	{
		return x * x + y * y;
	}

	Vec2<float> normalize()
	{
		float mag = magnitude();
		return Vec2<float>(x / mag, y / mag);
	}

	T dot(const Vec2<T>& other)
	{
		return x * other.x + y * other.y;
	}

	T cross(const Vec2<T>& other) const
	{
		return (x * other.y) - (y * other.x);
	}

	T x, y;
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

template<typename T>
struct Random
{
	static T range(const T& min, const T& max);
};