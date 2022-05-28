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
	static std::vector<Organism*> corpsesToSpawn;
	static std::vector<Organism*> parentsToClone;

	for (auto it = organisms_.begin(); it != organisms_.end();)
	{
		Organism* org = *it;
		org->tick();

		if (org->wantsToBeDeleted())
		{
			it = organisms_.erase(it);
			delete org;
		}
		else if (org->wantsToDie())
		{
			it = organisms_.erase(it);
			corpsesToSpawn.push_back(org->createCorpse());
			delete org;
		}
		else if (org->getReproductionCount() > 0)
		{
			for (unsigned i = 0; i < org->getReproductionCount(); i++)
				parentsToClone.push_back(org);

			org->setReproductionCount(0);
			++it;
		}
		else
			++it;
	}

	for (Organism* corpse : corpsesToSpawn)
		add(corpse);

	for (Organism* parent : parentsToClone)
		tryClone(parent);

	corpsesToSpawn.clear();
	parentsToClone.clear();
}

void OrganismManager::draw()
{
	for (Organism* org : organisms_)
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

void OrganismManager::tryClone(Organism* org)
{
	Vec2f spawnPos;

	if (org->isMover())
	{
		if (PhysicsManager::getInstance().findSpawnPosition(org->getAABB(), 2, spawnPos))
			add(org->clone(spawnPos));
	}
	else
	{
		if (PhysicsManager::getInstance().findSpawnPosition(org->getAABB(), 0, spawnPos))
			add(org->clone(spawnPos));
	}
}