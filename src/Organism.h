#pragma once

#include "Cell.h"

class Organism : IComponent
{
public:
	Organism(const std::string& dna);
	void draw() const;
	void tick(const float deltaTime);
	void set_position(const Vec2& pos);
	void set_velocity_linear(const Vec2& pos);
	void set_velocity_angular(float vel);
private:
	std::string m_dna;
	float m_energy = 100;
	std::vector<Cell*> m_cells;
	std::vector<glm::mat4> m_cell_offsets;
	Cell* m_mover_cell = nullptr;
	b2Body* m_body;
	Vec2 m_target_point = {200, 200};
	float m_speed_linear = 0;
	float m_speed_angular = 2;
private:
	bool overlaps_position(const Vec2& pos) const;
	constexpr Vec2 get_offset_for_param(const unsigned int param);
	constexpr Cell* get_cell_for_symbol(const char symbol);
	ClockTimer m_timer;	
	
};