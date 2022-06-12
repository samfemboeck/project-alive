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
	updateCorpses(corpsesPlants_);
	updateCorpses(corpsesMovers_);
	update(plants_);
	update(movers_);
}

void OrganismManager::update(std::vector<Organism*>& vec)
{	
	static std::vector<Organism*> corpsesToSpawn;
	static std::vector<Organism*> parentsToClone;

	for (auto it = vec.begin(); it != vec.end();)
	{
		Organism* org = *it;
		org->tick();

		if (org->wantsToDie() || !PhysicsManager::getInstance().getGrid().contains(org->getAABB()->bounds))
		{
			it = vec.erase(it);

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

void OrganismManager::updateCorpses(std::vector<Organism*>& vec)
{	
	for (auto it = vec.begin(); it != vec.end();)
	{
		Organism* org = *it;
		org->tick();

		if (org->wantsToDie())
		{
			it = vec.erase(it);
			delete org;
		}
		else
		{
			it++;
		}
	}
}

void OrganismManager::draw()
{
	for (Organism* org : corpsesMovers_)
		org->draw();
	
	for (Organism* org : corpsesPlants_)
		org->draw();

	for (Organism* org : plants_)
		org->draw();

	for (Organism* org : movers_)
		org->draw();
}

bool OrganismManager::add(Organism* org)
{
	PhysicsManager::getInstance().update(org->getAABB());

	if (org->isMover())
	{
		if (movers_.size() + corpsesMovers_.size() < MaxMovers && PhysicsManager::getInstance().hasValidPos(org->getAABB()))
		{
			movers_.push_back(org);
			PhysicsManager::getInstance().add(org->getAABB());
			return true;
		}

		return false;
	}
	else if (org->isCorpse() && PhysicsManager::getInstance().hasValidPos(org->getAABB())) // always force corpses to spawn
	{
		if (org->isMover())
			corpsesMovers_.push_back(org);
		else
			corpsesPlants_.push_back(org);

		PhysicsManager::getInstance().add(org->getAABB());
		return true;
	}
	else
	{
		if (plants_.size() + corpsesPlants_.size() < MaxPlants && PhysicsManager::getInstance().hasValidPos(org->getAABB()))
		{
			plants_.push_back(org);
			PhysicsManager::getInstance().add(org->getAABB());
			return true;
		}

		return false;
	}
}

bool OrganismManager::tryClone(Organism* org)
{
	Vec2f spawnPos;

	if (org->isMover())
	{
		if (PhysicsManager::getInstance().findSpawnPosition(org->getAABB(), org->getSize(), spawnPos))
		{
			Organism* clone = org->clone(spawnPos);

			if (clone)
			{
				if (!add(clone))
					delete clone;
			}

			return true;
		}
	}
	else
	{
		if (PhysicsManager::getInstance().findSpawnPosition(org->getAABB(), 0, spawnPos))
		{
			Organism* clone = org->clone(spawnPos);

			if (clone)
			{
				if (!add(clone))
					delete clone;
			}

			return true;
		}
	}

	return false;
}