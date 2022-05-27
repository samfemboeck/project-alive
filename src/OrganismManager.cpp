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
	std::vector<Organism*> to_delete;
	std::vector<int> results;

	for (auto it = organisms_.begin(); it != organisms_.end();)
	{
		if ((*it)->isDeleted())
		{
			delete* it;
			it = organisms_.erase(it);
		}
		else
			++it;
	}

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
	{
		if (org->isLeaf())
			continue;

		if (std::erase(organisms_, org) == 0)
			LOG("erase failed!");
	}


	for (unsigned i = 0; i < results.size(); i++)
	{
		Organism* target = to_delete[i];

		if (target->isLeaf())
			continue;

		add(target->createCorpse());
	}

	for (unsigned i = 0; i < results.size(); i++)
	{
		Organism* target = to_delete[i];

		for (unsigned a = 0; a < results[i]; a++)
			tryRespawn(target);

		if (target->isLeaf())
			continue;

		delete target;
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
	PhysicsManager::getInstance().update(org->getAABB());

	if (organisms_.size() < MaxInstances && PhysicsManager::getInstance().hasValidPos(org->getAABB()))
	{
		organisms_.push_back(org);
		PhysicsManager::getInstance().add(org->getAABB());
		return true;
	}

	delete org;
	return false;
}

bool OrganismManager::tryRespawn(Organism* org)
{
	Vec2f spawnPos;

	if (org->isMover())
	{
		if (PhysicsManager::getInstance().findSpawnPosition(org->getAABB(), 1, spawnPos))
		{
			add(org->clone(spawnPos));
			return true;
		}
	}
	else
	{
		if (PhysicsManager::getInstance().findSpawnPosition(org->getAABB(), 0, spawnPos))
		{
			add(org->clone(spawnPos));
			return true;
		}
	}

	return false;
}

void OrganismManager::remove(Organism* org)
{
	if (std::erase(organisms_, org) == 0)
		LOG("erase failed!");

	delete org;
}
