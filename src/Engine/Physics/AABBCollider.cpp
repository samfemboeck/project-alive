/*
#include "AABBCollider.h"
#include "CircleCollider.h"
#include "GameObject.h"
#include <memory>
#include <math.h>
#include <algorithm>
#include "PhysicsManager.h"
#include "PolygonCollider.h"
#include "QuickMaths.h"
#include <iostream>
// (c) Coding: Femböck Samuel, Hofer Thomas
// FH - Salzburg

using namespace hfm;

bool AABBCollider::collidesWith(Collider* collider, MTV& mtv)
{
	bool collides = false;

	AABBCollider* box = dynamic_cast<AABBCollider*>(collider);

	if (box)
	{
		return vsAABB(mShape, dynamic_cast<sf::RectangleShape& > (box->getShape()), mtv);
	}
	
	CircleCollider* circle = dynamic_cast<CircleCollider*>(collider);

	if (circle)
	{
		return vsCircle(mShape, dynamic_cast<sf::CircleShape&>(circle->getShape()), mtv);
	}
	

	PolygonCollider* Polygon = dynamic_cast<PolygonCollider*>(collider);

	if (Polygon)
	{
		return Polygon->vsPoly(getVertices(), { {1, 0}, {0, 1}, {-1, 0}, {0, -1} }, mShape.getPosition(),
			Polygon->getVertices(), Polygon->getAxes(), Polygon->getCenter(), mtv);
	}

	return false;
}

void AABBCollider::update(sf::Time)
{
	mShape.setPosition(mGo->getPosition() + getOffset());
}

sf::Shape& AABBCollider::getShape()
{
	return mShape;
}

std::vector<sf::Vector2f> AABBCollider::getVertices()
{
	std::vector<sf::Vector2f> ret;

	for (int i = 0; i < mShape.getPointCount(); ++i)
	{
		ret.push_back(mShape.getPosition() + mShape.getPoint(i));
	}

	return ret;
}

AABBCollider* AABBCollider::clone()
{
	return new AABBCollider(*this);
}

AABBCollider::AABBCollider(tld::Object* obj, tld::ObjectGroup* group, GameObject* go) : Collider(obj, group, go)
{
	mShape.setSize({ (float)obj->Width * Map::Scale, (float)obj->Height * Map::Scale });
	mShape.setPosition({ (float)obj->X * Map::Scale, (float)obj->Y * Map::Scale});

	if (group->Color)
		mShape.setOutlineColor(*group->Color);
	else
		mShape.setOutlineColor(sf::Color::Magenta);

	mShape.setOutlineThickness(2);
	mShape.setFillColor(sf::Color::Transparent);
	setOffset(sf::Vector2f(mShape.getPosition().x - go->getPosition().x, mShape.getPosition().y - go->getPosition().y));	
}

AABBCollider::AABBCollider(const AABBCollider& other) : Collider(other)
{
	mShape = other.mShape;
}

bool AABBCollider::vsAABB(sf::RectangleShape& a, sf::RectangleShape& b, MTV& mtv)
{
	sf::Vector2f aExtend = a.getSize() * 0.5f;
	sf::Vector2f bExtend = b.getSize() * 0.5f;
	auto centerA = a.getPosition() + aExtend;
	auto centerB = b.getPosition() + bExtend;
	auto distance = centerB - centerA;

	float xOverlap = aExtend.x + bExtend.x - abs(distance.x);
	if (xOverlap > 0)
	{
		float yOverlap = aExtend.y + bExtend.y - abs(distance.y);

		if (yOverlap > 0)
		{
			if (xOverlap < yOverlap)
			{
				mtv.normal = distance.x <= 0 ? sf::Vector2f(-1, 0) : sf::Vector2f(1, 0);
				mtv.penetration = xOverlap;
			}
			else
			{
				mtv.normal = distance.y <= 0 ? sf::Vector2f(0, -1) : sf::Vector2f(0, 1);
				mtv.penetration = yOverlap;
			}

			return true;
		}
	}

	return false;
}

bool AABBCollider::vsCircle(sf::RectangleShape& rect, sf::CircleShape& circle, MTV& mtv)
{
	sf::Vector2f circleCenter = sf::Vector2f(circle.getPosition().x + circle.getRadius(), circle.getPosition().y + circle.getRadius());

	float testx = circleCenter.x;
	float testy = circleCenter.y;

	if (circleCenter.x < rect.getPosition().x)
	{
		testx = rect.getPosition().x;
	}
	else if (circleCenter.x > rect.getPosition().x + rect.getSize().x)
	{
		testx = rect.getPosition().x + rect.getSize().x;
	}

	if (circleCenter.y < rect.getPosition().y)
	{
		testy = rect.getPosition().y;
	}
	else if (circleCenter.y > rect.getPosition().y + rect.getSize().y)
	{
		testy = rect.getPosition().y + rect.getSize().y;
	}

	float distx = circleCenter.x - testx;
	float disty = circleCenter.y - testy;
	float distance = sqrt((distx * distx) + (disty * disty));

	if (distance <= circle.getRadius()) 
	{
		if (abs(distx) > abs(disty)) 
		{
			if (distx != 0) {
				mtv.normal = sf::Vector2f(distx / (sqrt(distx * distx)), 0);
				mtv.penetration = circle.getRadius() - distance;
			}
		}
		else 
		{
			if (disty != 0)
			{
				mtv.normal = sf::Vector2f(0, disty / (sqrt(disty * disty)));
				mtv.penetration = circle.getRadius() - distance;
			}

		}
		return true;
	}

	return false;
}

sf::Vector2f AABBCollider::getCenter()
{
	return mShape.getPosition() + (mShape.getSize() * 0.5f);
}
*/
