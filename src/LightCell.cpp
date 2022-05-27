#include "pch.h"
#include "LightCell.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Renderer2D.h"
#include "LeafCell.h"
#include "Engine/QuickMaths.h"

LightCell::LightCell(Organism* org, CircleCollider* collider) :
	Cell(org, collider, "cell_light")
{
}

void LightCell::draw()
{
	Cell::draw();
	glm::translate(glm::mat4(1.0f), glm::vec3(1, 1, 1));
	Renderer2D::pushQuad(TRANSLATE(collider_->centerWorld) * SCALE(LightRadius * 2), TextureManager::get("light_circle.png"), glm::vec4(1.0f), false);
}

void LightCell::tick()
{
	return;
	queryResult_.clear();

	auto min = collider_->centerWorld - Vec2(LightRadius, LightRadius);
	auto max = collider_->centerWorld + Vec2(LightRadius, LightRadius);

	PhysicsManager::getInstance().squareCast(min, max, queryResult_, collider_->rigidBody);
}
