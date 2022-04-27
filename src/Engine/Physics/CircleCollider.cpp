#include "../../pch.h"
#include "CircleCollider.h"
#include "AABBCollider.h"
#include "PhysicsManager.h"
#include "RigidBody.h"

CircleCollider::CircleCollider(RigidBody* rb, float radius, glm::vec2 offset) : 
	Collider(rb, offset),
	Radius(radius)
{
}

bool CircleCollider::vs_circle(CircleCollider& a, CircleCollider& b, MTV& mtv)
{
	glm::vec2 difference = glm::vec2(a.Pos.x + a.Radius,a.Pos.y + a.Radius) 
		- glm::vec2(b.Pos.x + b.Radius, b.Pos.y + b.Radius);
	float length = glm::length(difference);
	float radiusSum = a.Radius + b.Radius;

	if (length == 0)
	{
		mtv.normal = glm::vec2(-1 , -1 );
		mtv.penetration = -1;
		return true;
	}

	if (length < radiusSum)
	{
		mtv.normal = glm::vec2(-1 / length * difference.x, -1 / length * difference.y);
		mtv.penetration = radiusSum - length;
		return true;
	}

	return false;

}

bool CircleCollider::collides_with(Collider* collider, MTV& mtv)
{
	/*
	AABBCollider* box = dynamic_cast<AABBCollider*>(collider);

	if (box)
	{
		return AABBCollider::vsCircle(dynamic_cast<sf::RectangleShape&>(box->getShape()), mShape, mtv);
	}

	*/
	CircleCollider* circle = dynamic_cast<CircleCollider*>(collider);

	if (circle)
	{
		return vs_circle(*this, *circle, mtv);
	}

	/*
	PolygonCollider* poly = dynamic_cast<PolygonCollider*>(collider);
	
	if (poly)
	{
		return PolygonCollider::vsCircle(*poly, *this, mtv);
	}
	*/

}

void CircleCollider::update()
{
	Pos = RB->Transform.Position + Offset;
}

void CircleCollider::draw()
{
	Renderer2D::pushQuad(glm::translate(glm::mat4(1.0f), { Pos, 0 }) * glm::scale(glm::mat4(1.0f), { Radius * 2, Radius * 2, 1 }), TextureManager::get("collider.png"), glm::vec4(1), false);
}
