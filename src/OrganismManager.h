#pragma once

class Organism;

class OrganismManager
{
public:
	static OrganismManager& getInstance();
	void update();
	void update(std::vector<Organism*>& vec, bool sort);
	void updateCorpses(std::vector<Organism*>& vec);
	void draw();
	bool add(Organism* org);
	bool tryClone(Organism* org);

public:
	std::vector<Organism*> corpsesMovers_;
	std::vector<Organism*> corpsesPlants_;
	std::vector<Organism*> movers_;
	std::vector<Organism*> plants_;
	inline static int MaxPlants = 10000;
	inline static float MaxMovers = 60;

private:
	OrganismManager() = default;
	float maxMovers_;
};