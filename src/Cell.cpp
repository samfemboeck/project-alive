#include "pch.h"
#include "Cell.h"
#include "Engine/Renderer2D.h"
#include "Engine/Physics.h"
#include "Organism.h"
#include "Engine/OrthoCamController.h"
#include "CorpseCell.h"

Cell::Cell(std::string textureName) :
	textureName_(textureName + ".png"),
	textureNameFilled_(textureName + "_filled.png")
{
	Instances++;
}

Cell::~Cell()
{
	Instances--;
}

void Cell::draw()
{
	Renderer2D::pushQuad(collider_->transform, TextureManager::get(textureNameFilled_), glm::vec4(1.0f), false);
}

void Cell::setOrganism(Organism* org)
{
	organism_ = org;
	collider_ = new CircleCollider();
	collider_->rigidBody = org->getRigidBody();
	collider_->radius = Cell::Size * 0.5f;
	collider_->centerLocal = localPos_;
	collider_->cell = this;
	collider_->collisionCallback = std::bind(&Cell::onCollision, this, std::placeholders::_1);
}

void Cell::setLocalPos(const Vec2f& pos)
{
	localPos_ = pos;
}

Vec2f Cell::getLocalPos() const
{
	return localPos_;
}

Organism* Cell::getOrganism() const
{
	return organism_;
}

CircleCollider* Cell::getCollider() const
{
	return collider_;
}

CorpseCell* Cell::createCorpse() const
{
	return nullptr;
}

void Cell::markForDeletion()
{
	wantsToBeDeleted_ = true;
}

bool Cell::wantsToBeDeleted()
{
	return wantsToBeDeleted_;
}

float Cell::getMass() const
{
	return 1.0f;
}

Type Cell::getType()
{
	return type_;
}
