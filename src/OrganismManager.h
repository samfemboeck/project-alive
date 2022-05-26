#pragma once

class Organism;

class OrganismManager
{
public:
	static OrganismManager& getInstance();
	void update();
	void draw();
	bool add(Organism* org);

public:
#ifdef _DEBUG
	inline static int MaxInstances = 1000;
#else
	inline static int MaxInstances = 8000;
#endif

private:
	OrganismManager() = default;

private:
	std::vector<Organism*> organisms_;
};