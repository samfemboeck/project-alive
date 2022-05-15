#include "pch.h"
#include "Cell.h"
#include "Engine/Renderer2D.h"
#include "Engine/Physics.h"
#include "Organism.h"

Cell::Cell(Organism* org, CircleCollider* collider, std::string textureName) :
	organism_(org),
	collider_(collider),
	textureName_(textureName)
{
}

Cell::~Cell()
{
	Instances--;
	PhysicsManager::getInstance().remove(collider_);
}

void Cell::draw()
{
	Renderer2D::pushQuad(collider_->transform, TextureManager::get(textureName_), glm::vec4(1.0f), false);
}
