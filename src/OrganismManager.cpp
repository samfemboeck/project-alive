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
	update(plants_, false);
	update(movers_, true);

	float avgSize = 2;

	if (movers_.size() > 0)
	{
		float sumSize = 0;

		for (Organism* mover : movers_)
			sumSize += mover->getSize();

		avgSize = sumSize / movers_.size();
	}

	maxMovers_ = 300.0f / avgSize;
}

bool compareReproductionUrge(Organism* org1, Organism* org2)
{
    return (org1->getReproductionUrge() > org2->getReproductionUrge());
}

void OrganismManager::update(std::vector<Organism*>& vec, bool sort)
{	
	static std::vector<Organism*> corpsesToSpawn;
	static std::vector<Organism*> parentsToClone;

	for (auto it = vec.begin(); it != vec.end();)
	{
		Organism* org = *it;
		org->tick();

		if (org->wantsToDie())
		{
			it = vec.erase(it);

			Organism* corpse = org->createCorpse();
			if (corpse)
				corpsesToSpawn.push_back(corpse);

			delete org;
		}
		else
		{		
			unsigned numOffspring = static_cast<unsigned>(org->getReproductionUrge());

			for (unsigned i = 0; i < numOffspring; i++)
				parentsToClone.push_back(org);

			it++;
		}
	}

	for (Organism* corpse : corpsesToSpawn)
		if (!add(corpse))
			delete corpse;

	if (sort)
		std::sort(parentsToClone.begin(), parentsToClone.end(), compareReproductionUrge);

	for (Organism* parent : parentsToClone)
	{
		if (tryClone(parent))
			parent->setEnergy(parent->getEnergy() - parent->getHunger());
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
	for (Organism* org : corpsesPlants_)
		org->draw();

	for (Organism* org : corpsesMovers_)
		org->draw();

	for (Organism* org : plants_)
		org->draw();

	for (Organism* org : movers_)
		org->draw();
}

bool OrganismManager::add(Organism* org)
{
	PhysicsManager::getInstance().update(org->getAABB());

	if (org->isMover() && !org->isCorpse())
	{
		if (movers_.size() < maxMovers_ && PhysicsManager::getInstance().hasValidPos(org->getAABB()))
		{
			movers_.push_back(org);
			PhysicsManager::getInstance().add(org->getAABB());
			return true;
		}

		return false;
	}
	else if (org->isCorpse() && PhysicsManager::getInstance().hasValidPos(org->getAABB()))	
	{
		if (org->isMover() && corpsesMovers_.size() < maxMovers_)
			corpsesMovers_.push_back(org);
		else if (corpsesPlants_.size() + plants_.size() < MaxPlants)
			corpsesPlants_.push_back(org);
		else 
			return false;

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
		if (PhysicsManager::getInstance().findSpawnPosition(org->getAABB(), org->getSize() + 1, spawnPos))
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