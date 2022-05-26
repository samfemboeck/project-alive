#include "pch.h"
#include "OrganismManager.h"
#include "Engine/Timer.h"
#include "Organism.h"
#include "Engine/Physics.h"

OrganismManager& OrganismManager::getInstance()
{
	static OrganismManager manager;
	return manager;
}

void OrganismManager::update()
{
	ScopeTimer timer("Tick Organisms");
	static std::vector<Organism*> to_delete;
	static std::vector<int> results;

	for (auto* org : organisms_)
	{
		int result = org->tick();

		if (result != -1)
		{
			results.push_back(result);
			to_delete.push_back(org);
		}
	}

	for (auto* org : to_delete)
		std::erase(organisms_, org);

	for (unsigned i = 0; i < results.size(); i++)
	{
		Organism* target = to_delete[i];
		add(target->createCorpse());
	}

	for (unsigned i = 0; i < results.size(); i++)
	{
		Organism* target = to_delete[i];
		auto* aabb = target->getAABB();

		for (unsigned a = 0; a < results[i]; a++)
		{
			Vec2f spawnPos;
			if (PhysicsManager::getInstance().findSpawnPosition(aabb, 2, spawnPos))
				add(to_delete[i]->clone(spawnPos));
		}

		delete to_delete[i];
	}

	to_delete.clear();
	results.clear();
}

void OrganismManager::draw()
{
	for (auto* org : organisms_)
		org->draw();
}

bool OrganismManager::add(Organism* org)
{
	if (organisms_.size() < MaxInstances)
	{
		LOG("MaxInstances: {}", MaxInstances);
		organisms_.push_back(org);
		return true;
	}

	delete org;
	return false;
}
