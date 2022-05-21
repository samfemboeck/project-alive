#include "pch.h"
#include "LightCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Renderer2D.h"
#include "LeafCell.h"

LightCell::LightCell(Organism* org, CircleCollider* collider) :
	Cell(org, collider, "cell_light")
{
}

void LightCell::draw()
{
	Cell::draw();
	glm::translate(glm::mat4(1.0f), glm::vec3(1, 1, 1));
	Renderer2D::pushQuad(TRANSLATE(collider_->centerWorld) * SCALE(LightRadius), TextureManager::get("light_circle.png"), glm::vec4(1.0f), false);
}

void LightCell::tick()
{
	queryResult_.clear();
	auto min = organism_->rigidBody_->position - Vec2(LightRadius, LightRadius);
	auto max = organism_->rigidBody_->position + Vec2(LightRadius, LightRadius);

	if (PhysicsManager::getInstance().squareCast(min, max, queryResult_))
	{
		for (AABB* result : queryResult_)
		{
			if (result->rigidBody == organism_->rigidBody_)
				continue;

			auto colliders = result->rigidBody->colliders;
			for (auto* collider : colliders)
			{
				if (collider->cell)
				{
					LeafCell* leafCell = dynamic_cast<LeafCell*>(collider->cell);
					if (leafCell)
						leafCell->setLit(true);
				}
			}
		}
	}
}
