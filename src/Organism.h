#pragma once

#include "Cell.h"

struct Organism : IComponent
{
	static constexpr size_t MaxInstances = 200;
	Organism(const std::string& dna);
	void draw() const;
	void tick();
	void set_transform(Vec2 pos, float rot);
private:
	std::string m_dna;
	std::vector<Cell*> m_cells;
	Cell* m_mover_cell = nullptr;
	float m_velocity = 0;
	b2Body* m_body;
private:
	bool overlaps_position(const Vec2& pos) const;
	Vec2 get_offset_for_param(const unsigned param) const;
	Cell* get_cell_for_symbol(const char symbol) const;
	ClockTimer m_timer_ttl;	
	std::function<float(float)> m_instinct;
};
