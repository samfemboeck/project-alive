#pragma once

#include "Cell.h"
#include "Engine/Physics/RigidBody.h"
#include "Engine/Physics/CircleCollider.h"

class Organism
{
public:
	Organism(const std::string& dna);
	void draw() const;
	void tick(const float deltaTime);
	void set_position(const glm::vec2& pos);
	void set_velocity(const glm::vec2& pos);
private:
	RigidBody m_rigid_body;
	std::string m_dna;
	float m_energy = 100;
	std::vector<Cell*> m_cells;
	std::vector<glm::mat4> m_cell_offsets;
	Cell* m_mover_cell = nullptr;
	CircleCollider* m_circle_collider;
private:
	bool overlaps_position(const glm::vec2& pos) const;
	constexpr glm::vec2 get_offset_for_param(const unsigned int param);
	constexpr Cell* get_cell_for_symbol(const char symbol);
};