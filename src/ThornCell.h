#pragma once

#include "Cell.h"

class ThornCell final : public Cell
{
public:
	ThornCell(Organism*, CircleCollider*);
};