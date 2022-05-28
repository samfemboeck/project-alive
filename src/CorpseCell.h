#pragma once

#include "Cell.h"

class Organism;
struct CircleCollider;

class CorpseCell final : public Cell
{
public:
	CorpseCell(Organism*, CircleCollider*);
	void tick() override;
};