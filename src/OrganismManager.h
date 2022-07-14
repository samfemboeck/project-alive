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
	const std::unordered_map<std::string, unsigned>& getRegistry();
	unsigned getNumPlantCorpses();
	std::string getDominatingSpecies();
	bool getPredatorEventTriggered();
	bool getParasiteEventTriggered();
	bool getThornEventTriggered();
	bool getFiveCellDominationEventTriggered();
	bool getPredatorDominationEventTriggered();
	bool getThornCellDiscovered();
	bool getHerbivoreDiscovered();
	bool getCarnivoreDiscovered();

public:
	inline static int MaxPlants = 10000;
	inline static float MaxMovers = 60;

private:
	OrganismManager() = default;

private:
	std::vector<Organism*> corpsesMovers_;
	std::vector<Organism*> corpsesPlants_;
	std::vector<Organism*> movers_;
	std::vector<Organism*> plants_;
	float maxMovers_;
	std::unordered_map<std::string, unsigned> registry_;
	std::unordered_map<std::string, unsigned> registryPlants_;
	std::string dominatingSpecies_ = "MO";
	bool predatorEventTriggered_ = false;
	bool parasiteEventTriggered_ = false;
	bool thornEventTriggered_ = false;
	bool fiveCellDominationEventTriggered_ = false;
	bool predatorDominationEventTriggered_ = false;
	bool thornCellDiscovered_ = false;
	bool mouthCellDiscovered_ = false;
	bool herbivoreDiscovered_ = false;
	bool carnivoreDiscovered_ = false;
};