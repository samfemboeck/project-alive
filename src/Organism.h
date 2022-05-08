#pragma once

#include "Engine/QuickMaths.h"
#include "Cell.h"
#include "Engine/Timer.h"

struct Organism : IComponent
{
	static constexpr unsigned MAX_INSTANCES = 100;
	inline static long TTL = 1000;

	inline static std::vector<std::string> DEFAULT_DNAS = {
		"L(0)",
		"T(0)M(1)L(0)",
		"A(0)", 
		"L(0)T(0)",
	};

	inline static unsigned DNA_INDEX = 0;
	inline static std::vector<Organism*> INSTANCES;

	Organism(const std::string& dna, std::function<float(float)> instinct, Vec2 position, float angle);
	~Organism();
	void draw() const;
	void draw_light() const;
	int tick();
	void set_transform(Vec2 pos, float rot);
	void destroy();
	Organism* clone();
	
private:
	std::string m_dna;
	std::vector<Cell*> m_cells;
	Cell* m_mover_cell = nullptr;
	Cell* m_light_cell = nullptr;
	b2Body* m_body;
private:
	friend struct LeafCell;
	bool overlaps_position(const Vec2& pos) const;
	Vec2 get_offset_for_param(const unsigned param) const;
	Cell* get_cell_for_symbol(const char symbol) const;
	std::function<float(float)> m_instinct;
	long m_initial_ttl;
	TickCountTimer m_timer_ttl;
};
