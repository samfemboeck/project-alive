#pragma once

#include "Cell.h"

class MoverCell final : public Cell
{
public:
	MoverCell(Organism* org, CircleCollider* collider);
	void tick() override;
};