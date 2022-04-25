#include "../../pch.h"
#include "CircleCollider.h"
#include "AABBCollider.h"
#include "PhysicsManager.h"
#include "RigidBody.h"

/*
void CircleCollider::update(sf::Time)
{
	mShape.setPosition(mGo->getPosition() + getOffset());
}
*/

CircleCollider::CircleCollider(RigidBody* rb, float radius, glm::vec2 offset) : 
	Collider(rb, offset),
	m_radius(radius)
{
}

bool CircleCollider::vs_circle(CircleCollider& a, CircleCollider& b, MTV& mtv)
{
	glm::vec2 difference = glm::vec2(a.m_pos.x + a.m_radius,a.m_pos.y + a.m_radius) 
		- glm::vec2(b.m_pos.x + b.m_radius, b.m_pos.y + b.m_radius);
	float length = glm::length(difference);
	float radiusSum = a.m_radius + b.m_radius;

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
	m_pos = pm_rigid_body->pm_transform.pm_position + m_offset;
}

void CircleCollider::draw()
{
	Renderer2D::pushQuad(glm::translate(glm::mat4(1.0f), { m_pos, 0 }) * glm::scale(glm::mat4(1.0f), { m_radius * 2, m_radius * 2, 1 }), TextureManager::get("collider.png"), glm::vec4(1), false);
}

/*
CircleCollider::CircleCollider(tld::Object* obj, tld::ObjectGroup* group, GameObject* go) : Collider(obj, group, go)
{
	mShape.setRadius(obj->Height * 0.5f * Map::Scale);
	mShape.setPosition({ (float)obj->X * Map::Scale, (float)obj->Y * Map::Scale});

	if (group->Color)
		mShape.setOutlineColor(*group->Color);
	else
		mShape.setOutlineColor(sf::Color::Magenta);

	mShape.setOutlineThickness(2);
	mShape.setFillColor(sf::Color::Transparent);
	setOffset(sf::Vector2f(mShape.getPosition().x - go->getPosition().x, mShape.getPosition().y - go->getPosition().y));	
}
*/
