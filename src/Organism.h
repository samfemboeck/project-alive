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
	Vec2 m_target_point;
private:
	std::string m_dna;
	float m_energy = 100;
	std::vector<Cell*> m_cells;
	std::vector<glm::mat4> m_cell_offsets;
	Cell* m_mover_cell = nullptr;
	b2Body* m_body;
	float m_speed_linear = 0;
	float m_speed_angular = 8;
	float m_elapsed = Random<float>::range(0, 6);
	float m_multiplier_sin = Random<float>::range(1, 5);
	float m_multiplier_velocity_angular = Random<float>::range(0, 10);
	float m_multiplier_velocity_linear = Random<float>::range(0, 10);
private:
	bool overlaps_position(const Vec2& pos) const;
	Vec2 get_offset_for_param(const unsigned param) const;
	Cell* get_cell_for_symbol(const char symbol) const;
	ClockTimer m_timer_ttl;	
	std::vector<std::function<float(void)>> m_instincts;
	
};