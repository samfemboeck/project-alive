#pragma once

class Organism;

class OrganismManager
{
public:
	static OrganismManager& getInstance();
	void update();
	void update(std::vector<Organism*>& vec);
	void updateCorpses(std::vector<Organism*>& vec);
	void draw();
	bool add(Organism* org);
	bool tryClone(Organism* org);

public:
#ifdef _DEBUG
	inline static int MaxPlants = 200;
	inline static int MaxMovers = 200;
#else
	inline static int MaxPlants = 2500;
	inline static int MaxMovers = 500;
#endif

private:
	OrganismManager() = default;

private:
	std::vector<Organism*> corpsesMovers_;
	std::vector<Organism*> corpsesPlants_;
	std::vector<Organism*> movers_;
	std::vector<Organism*> plants_;
};