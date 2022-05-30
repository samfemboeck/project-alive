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

		if (org->wantsToDie() || !PhysicsManager::getInstance().getGrid().contains(org->getAABB()->bounds))
		{
			it = organisms_.erase(it);

			Organism* corpse = org->createCorpse();
			if (corpse)
				corpsesToSpawn.push_back(corpse);

			delete org;
		}
		else if (org->getReproductionUrge() > 0)
		{
			for (unsigned i = 0; i < org->getReproductionUrge(); i++)
				parentsToClone.push_back(org);
			++it;
		}
		else
			++it;
	}

	for (Organism* corpse : corpsesToSpawn)
		add(corpse);

	for (Organism* parent : parentsToClone)
	{
		if (tryClone(parent))
			parent->setReproductionUrge(parent->getReproductionUrge() - 1);
	}

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

bool OrganismManager::tryClone(Organism* org)
{
	Vec2f spawnPos;

	if (org->isMover())
	{
		if (PhysicsManager::getInstance().findSpawnPosition(org->getAABB(), 2, spawnPos))
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
}