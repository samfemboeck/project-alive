#pragma once

#include "Cell.h"

class Organism
{
public:
	Organism(std::string dna);
	void tick(float deltaTime);
	void draw();
	RigidBody m_rigid_body;
private:
	std::string m_dna;
	float m_energy = 100;
	std::vector<Cell*> m_cells;
	std::vector<glm::mat4> m_cell_offsets;
	Cell* m_mover_cell = nullptr;
	CircleCollider* m_circle_collider;
private:
	bool overlaps_position(glm::vec2 pos);
	glm::vec2 get_offset_for_param(unsigned int param);
	Cell* get_cell_for_symbol(char symbol);
};