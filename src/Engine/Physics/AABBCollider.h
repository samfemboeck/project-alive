/*
#pragma once
#include "Collider.h"

namespace hfm
{
	class AABBCollider : public Collider
	{
	public:
		static bool vsCircle(sf::RectangleShape& rect, sf::CircleShape& circle, MTV& mtv);
		static bool vsAABB(sf::RectangleShape& a, sf::RectangleShape& b, MTV& mtv);

		AABBCollider(tld::Object* obj, tld::ObjectGroup* group, GameObject* go);
		AABBCollider(const AABBCollider& other);
		AABBCollider() = delete;

		AABBCollider* clone() override;
		bool collidesWith(Collider* collider, MTV& mtv) override;
		void update(sf::Time) override;
		sf::Shape& getShape() override;

		sf::Vector2f getCenter();
		std::vector<sf::Vector2f> getVertices();

	private:
		sf::RectangleShape mShape;
	};
}
*/
