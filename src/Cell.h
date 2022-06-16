#pragma once
#include "Engine/QuickMaths.h"

class Organism;
struct CircleCollider;
class CorpseCell;

enum class Type
{
	None,
	Mouth,
	Mover,
	Plant
};

class Cell
{
public:
	inline static int Instances = 0;
	inline static const float Size = 40;

public:
	Cell(std::string textureName);
	virtual void tick() {};
	virtual ~Cell();
	virtual void onCollision(Cell* other) {}
	virtual void draw();
	virtual void init() {};
	void setOrganism(Organism* org);
	void setLocalPos(const Vec2f& pos);
	Vec2f getLocalPos() const;
	Organism* getOrganism() const;
	CircleCollider* getCollider() const;
	virtual CorpseCell* createCorpse() const;
	void markForDeletion();
	bool wantsToBeDeleted();
	virtual float getMass() const;
	Type getType();

protected:
	Organism* organism_ = nullptr;
	CircleCollider* collider_ = nullptr;
	std::string textureName_;
	std::string textureNameFilled_;
	Vec2f localPos_;
	bool wantsToBeDeleted_ = false;
	Type type_ = Type::None;
};