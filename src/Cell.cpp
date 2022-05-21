#include "pch.h"
#include "Cell.h"
#include "Engine/Renderer2D.h"
#include "Engine/Physics.h"
#include "Organism.h"
#include "Engine/OrthoCamController.h"

Cell::Cell(Organism* org, CircleCollider* collider, std::string textureName) :
	organism_(org),
	collider_(collider),
	textureName_(textureName + ".png"),
	textureNameFilled_(textureName + "_filled.png")
{
}

Cell::~Cell()
{
	Instances--;
	PhysicsManager::getInstance().remove(collider_);
}

void Cell::draw()
{
	if (OrthoCamController::getInstance().getZoomLevel() <= 1.5f)
		Renderer2D::pushQuad(collider_->transform, TextureManager::get(textureName_), glm::vec4(1.0f), false);
	else
		Renderer2D::pushQuad(collider_->transform, TextureManager::get(textureNameFilled_), glm::vec4(1.0f), false);
}
