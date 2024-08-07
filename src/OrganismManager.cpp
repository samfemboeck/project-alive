#include "pch.h"
#include "OrganismManager.h"
#include "Engine/Timer.h"
#include "Organism.h"
#include "Engine/Physics.h"
#include "Engine/Renderer2D.h"
#include "Engine/OrthoCamController.h"
#include "Engine/SoundManager.h"
#include "Main.h"

void OrganismManager::update()
{	
	// minimize performance overhead for corpses
	updateCorpses(corpsesPlants_);
	updateCorpses(corpsesMovers_);
	updateOrganisms(plants_, false); // We don't care about the most productive plants for optimization.
	updateOrganisms(movers_, true); // Survival of the fittest (most productive).
	updateEventTriggers();

	float avgSize = 2;

	if (!movers_.empty())
	{
		float sumSize = 0;

		for (Organism* mover : movers_)
			sumSize += mover->getSize();

		avgSize = sumSize / movers_.size();
	}

	maxMovers_ = 10000;
}

void OrganismManager::draw()
{
	auto& orthoCam = OrthoCamController::getInstance();
	Renderer2D::beginTextures(orthoCam.getView(), orthoCam.getProjection());

	for (Organism* org : corpsesPlants_) org->draw();
	for (Organism* org : corpsesMovers_) org->draw();
	for (Organism* org : plants_) org->draw();
	for (Organism* org : movers_) org->draw();

	Renderer2D::endTextures();
}

void OrganismManager::updateOrganisms(std::vector<Organism*>& orgs, bool prioritizeMostProductive)
{
	std::vector<Organism*> corpsesToSpawn;
	std::vector<Organism*> parentsToClone;

	for (auto it = orgs.begin(); it != orgs.end();)
	{
		Organism* org = *it;
		org->tick();

		if (org->wantsToDie())
		{
			it = orgs.erase(it);

			Organism* corpse = org->createCorpse();
			if (corpse)
				corpsesToSpawn.push_back(corpse);

			if (org->isMover() || org->isHerbivore())
				registry_[org->getDNA().str()] -= 1;
			else
				registryPlants_[org->getDNA().str()] -= 1;

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

	if (prioritizeMostProductive)
		std::sort(parentsToClone.begin(), parentsToClone.end(), [](Organism* org1, Organism* org2){ return org1->getReproductionUrge() > org2->getReproductionUrge(); });

	for (Organism* parent : parentsToClone)
		if (tryClone(parent))
			parent->setEnergy(parent->getEnergy() - parent->getHunger());
}

void OrganismManager::updateEventTriggers()
{
	unsigned numCarnivores = 0;
	unsigned numHerbivores = 0;
	unsigned numHerbivoresMoreThan4Cells = 0;
	unsigned numHerbivoresTotal = 0;

	for (const auto& pair : registry_)
	{
		if (pair.second > movers_.size() * 0.5f)
		{
			dominatingSpecies_ = pair.first;
		}

		if (!predatorEventTriggered_ && pair.first.contains('C') && pair.second >= 6)
		{
			predatorEventTriggered_ = true;
		}

		if (!parasiteEventTriggered_ && pair.first.contains('O') && !pair.first.contains('M') && pair.second >= 20)
		{
			parasiteEventTriggered_ = true;
		}

		if (!predatorDominationEventTriggered_)
		{
			if (pair.first.contains('C'))
				numCarnivores += pair.second;
			else if (pair.first.contains('O') && pair.first.contains('M'))
				numHerbivores += pair.second;
		}

		if (!fiveCellDominationEventTriggered_)
		{
			if (pair.first.contains('O') && pair.first.contains('M'))
			{
				std::string::difference_type numMs = std::count(pair.first.begin(), pair.first.end(), 'M');
				std::string::difference_type numOs = std::count(pair.first.begin(), pair.first.end(), 'O');

				if (numMs + numOs > 4)
					numHerbivoresMoreThan4Cells += pair.second;

				numHerbivoresTotal += pair.second;
			}
		}
	}

	if (!predatorDominationEventTriggered_ && numCarnivores > ((numCarnivores + numHerbivores) * (1 / 3.0f)))
		predatorDominationEventTriggered_ = true;

	if (!fiveCellDominationEventTriggered_ && numHerbivoresMoreThan4Cells > numHerbivoresTotal * 0.5f)
	{
		fiveCellDominationEventTriggered_ = true;
	}

	if (!thornEventTriggered_)
	{
		unsigned numThorns = 0;
		unsigned numTotal = 0;

		for (const auto& pair : registryPlants_)
		{
			if (pair.first.contains('T'))
				numThorns += pair.second;

			numTotal += pair.second;
		}

		if (registryPlants_.size() > 0 && numThorns >= numTotal * 0.5f)
		{
			thornEventTriggered_ = true;
		}
	}
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

bool OrganismManager::add(Organism* org)
{
	PhysicsManager::getInstance().update(org->getAABB());

	if ((org->isMover() || org->isHerbivore()) && !org->isCorpse())
	{
		if (movers_.size() < maxMovers_ && PhysicsManager::getInstance().hasValidPos(org->getAABB()))
		{
			movers_.push_back(org);
			PhysicsManager::getInstance().add(org->getAABB());
			registry_[org->getDNA().str()] += 1;
			return true;
		}

		return false;
	}
	else if (org->isCorpse() && PhysicsManager::getInstance().hasValidPos(org->getAABB()))
	{
		if (org->isMover() && corpsesMovers_.size() < maxMovers_)
		{
			corpsesMovers_.push_back(org);
			PhysicsManager::getInstance().add(org->getAABB());
			return true;
		}
		else if (corpsesPlants_.size() + plants_.size() < MaxPlants)
		{
			corpsesPlants_.push_back(org);
			PhysicsManager::getInstance().add(org->getAABB());
			return true;
		}

		return false;
	}
	else
	{
		if (plants_.size() + corpsesPlants_.size() < MaxPlants && PhysicsManager::getInstance().hasValidPos(org->getAABB()))
		{
			if (!thornCellDiscovered_ && org->isThorn())
				thornCellDiscovered_ = true;

			plants_.push_back(org);
			PhysicsManager::getInstance().add(org->getAABB());
			registryPlants_[org->getDNA().str()] += 1;
			return true;
		}

		return false;
	}
}

bool OrganismManager::tryClone(Organism* org)
{
	Vec2f spawnPos;

	if (!org->isMover() && org->isHerbivore()) // Non-moving Herbivore cell get VIP treatment so that they can flourish
	{
		if (PhysicsManager::getInstance().findSpawnPosition(org->getAABB(), 1000, spawnPos))
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
	else if (org->isMover())
	{
		if (PhysicsManager::getInstance().findSpawnPosition(org->getAABB(), org->getSize(), spawnPos))
		{
			Organism* clone = org->clone(spawnPos);

			if (clone)
			{
				if (!add(clone))
				{
					delete clone;
				}
				else
				{
					if (!herbivoreDiscovered_ && org->isHerbivore())
					{
						herbivoreDiscovered_ = true;
					}

					if (!carnivoreDiscovered_ && org->isCarnivore())
					{
						carnivoreDiscovered_ = true;
					}
				}
			}

			return true;
		}
	}
	else
	{
		if (PhysicsManager::getInstance().findSpawnPosition(org->getAABB(), 0, spawnPos))
		{
			static bool firstClone = true;

			if (firstClone)
			{
				AliveApp::getInstance().startSound();
				firstClone = false;
			}

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

const std::unordered_map<std::string, unsigned>& OrganismManager::getRegistry() { return registry_; }

unsigned OrganismManager::getNumPlantCorpses() { return corpsesPlants_.size(); }

std::string OrganismManager::getDominatingSpecies() { return dominatingSpecies_; }

bool OrganismManager::getPredatorEventTriggered() { return predatorEventTriggered_; }

bool OrganismManager::getParasiteEventTriggered() { return parasiteEventTriggered_; }

bool OrganismManager::getThornEventTriggered() { return thornEventTriggered_; }

bool OrganismManager::getFiveCellDominationEventTriggered() { return fiveCellDominationEventTriggered_; }

bool OrganismManager::getPredatorDominationEventTriggered() { return predatorDominationEventTriggered_; }

bool OrganismManager::getThornCellDiscovered() { return thornCellDiscovered_; }

bool OrganismManager::getHerbivoreDiscovered() { return herbivoreDiscovered_; }

bool OrganismManager::getCarnivoreDiscovered() { return carnivoreDiscovered_; }

OrganismManager& OrganismManager::getInstance() { static OrganismManager manager; return manager; }