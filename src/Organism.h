#pragma once

#include "Engine/QuickMaths.h"
#include "Cell.h"
#include "Engine/Timer.h"

class RigidBody;
class CircleCollider;

struct Organism : IComponent
{
public:
	static constexpr unsigned MAX_INSTANCES = 2000;
	inline static long TTL = 2000;

	inline static std::vector<std::string> DEFAULT_DNAS = {
		"L(0)",
		"T(0)M(1)L(0)",
		"A(0)", 
		"L(0)T(0)",
	};

	inline static unsigned DNA_INDEX = 0;
	inline static std::vector<Organism*> INSTANCES;

public:
	Organism(const std::string& dna, std::function<float(float)> instinct, Vec2 position, float angle);
	~Organism();
	void draw() const;
	void draw_light() const;
	int tick();
	void destroy();
	Organism* clone();
	RigidBody* m_rb;

private:
	friend struct LeafCell;
	
private:
	std::string m_dna;
	std::vector<Cell*> m_cells;
	std::vector<CircleCollider*> m_colliders;
	Cell* m_mover_cell = nullptr;
	Cell* m_light_cell = nullptr;
	std::function<float(float)> m_instinct;
	long m_initial_ttl;
	TickCountTimer m_timer_ttl;
	ClockTimer m_timer_clock;

private:
	bool overlaps_position(const Vec2& pos) const;
	Vec2 get_offset_for_param(const unsigned param) const;
	Cell* get_cell_for_symbol(const char symbol) const;
};
