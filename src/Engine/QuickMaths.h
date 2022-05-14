#pragma once

#include <glm/glm.hpp>
#include <string>

struct Vec2
{	
	Vec2() = default;
	Vec2(float x, float y);
	Vec2(const b2Vec2& vec);
	Vec2(const glm::vec2& vec);
	operator const glm::vec2() const;
	operator const b2Vec2() const;
	std::string str() const;
	Vec2& operator += (const Vec2& rhs);
	Vec2 operator+(const Vec2& rhs) const;
	Vec2 operator*(float rhs) const;
	Vec2 operator-(const Vec2& rhs) const;
	float magnitude();
	float magnitude_squared();
	Vec2 normalize();
	float dot(const Vec2& other);
	float cross(const Vec2& other) const;

	float x, y;
};

template<typename T>
struct Random
{
	static T range(const T& min, const T& max);
};