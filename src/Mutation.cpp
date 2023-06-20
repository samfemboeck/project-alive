#include "pch.h"
#include "Mutation.h"
#include "Engine/QuickMaths.h"
#include "OrganismManager.h"

std::vector<char> Production::mover(unsigned numMaxMutations)
{
	std::vector<std::string> vocabulary = { "M", "[M]", "[LM]","[RM]","M", "[M]", "[LM]","[RM]", "O", "[O]", "[LO]", "[RO]", "C", "[C]", "[LC]", "[RC]"};
	std::vector<char> ret;
	unsigned numInstructions = Random::unsignedRange(1, std::min(2u, numMaxMutations));

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = Random::unsignedRange(0, vocabulary.size() - 1);
		std::string& str = vocabulary[idx];
		for (char c : str)
			ret.push_back(c);
	}

	return ret;
}

std::vector<char> Production::plant()
{
	std::vector<std::string> vocabulary = { "P", "[P]", "[LP]", "[RP]","P", "[P]", "[LP]", "[RP]","[LP]", "[RP]", "T", "[T]", "[LT]", "[RT]" };
	//std::vector<std::string> vocabulary = { "P", "LP", "RP", "[P]", "[LP]", "[RP]","[LP]", "[RP]" };
	std::vector<char> ret;
	unsigned numInstructions = Random::unsignedRange(1, 1);

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = Random::unsignedRange(0, vocabulary.size() - 1);
		std::string& str = vocabulary[idx];
		for (char c : str)
			ret.push_back(c);
	}

	return ret;
}

std::vector<char> Production::mouth(unsigned numMaxMutations)
{	
	std::vector<std::string> vocabulary = { "O", "O", "[O]", "[O]", "[LO]", "[RO]", "M", "M", "[M]", "[M]", "[LM]", "[RM]", "C", "C", "[C]", "[C]", "[LC]", "[RC]"};
	std::vector<char> ret;
	unsigned numInstructions = Random::unsignedRange(1, std::min(2u, numMaxMutations));

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = Random::unsignedRange(0, vocabulary.size() - 1);
		std::string& str = vocabulary[idx];
		for (char c : str)
			ret.push_back(c);
	}

	return ret;
}

std::vector<char> Production::thorn()
{	
	std::vector<std::string> vocabulary = { "T", "[T]", "[LT]", "[RT]", "P", "[P]", "[LP]", "[RP]"};
	std::vector<char> ret;
	unsigned numInstructions = Random::unsignedRange(1, 1);

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = Random::unsignedRange(0, vocabulary.size() - 1);
		std::string& str = vocabulary[idx];
		for (char c : str)
			ret.push_back(c);
	}

	return ret;
}

std::vector<char> Production::carnivore(unsigned numMaxMutations)
{
	std::vector<std::string> vocabulary = { "C", "[C]", "[LC]", "[RC]", "C", "[C]", "[LC]", "[RC]", "M", "[M]", "[LM]", "[RM]" };
	std::vector<char> ret;
	unsigned numInstructions = Random::unsignedRange(1, std::min(2u, numMaxMutations));

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = Random::unsignedRange(0, vocabulary.size() - 1);
		std::string& str = vocabulary[idx];
		for (char c : str)
			ret.push_back(c);
	}

	return ret;
}

DNA::DNA(const std::vector<char>& elems) :
	elems_(elems),
	str_(std::string(elems.begin(), elems.end()))
{
	for (char c : elems)
	{
		if (c == 'M' || c == 'O' || c == 'C' || c == 'P' || c == 'T')
			numCells_++;

		if (c == 'M')
			isMover_ = true;

		if (c == 'O')
			isHerbivore_ = true;

		if (c == 'C')
			isCarnivore_ = true;
	}
}

DNA::DNA(std::string dna)
{
	for (char c : dna)
	{
		if (c == 'M' || c == 'O' || c == 'C' || c == 'P' || c == 'T')
			numCells_++;

		if (c == 'M')
			isMover_ = true;

		if (c == 'O')
			isHerbivore_ = true;

		if (c == 'C')
			isCarnivore_ = true;

		elems_.push_back(c);
	}

	str_ = std::string(elems_.begin(), elems_.end());
}

void DNA::mutate()
{		
	auto rng = Random::unsignedRange(0, 15);

	if (!isMover_ && OrganismManager::getInstance().getHerbivoreDiscovered() && rng == 0)
	{
		elems_.clear();
		elems_.push_back('M');
		elems_.push_back('O');
		str_ = get();
		return;
	}

	if (!isMover_ && OrganismManager::getInstance().getHerbivoreDiscovered() && rng == 1)
	{
		elems_.clear();
		elems_.push_back('O');
		str_ = get();
		return;
	}

	if (elems_.size() > 1 && Random::unsignedRange(0, 1) == 0)
	{
		unsigned numInstructions = Random::unsignedRange(1, 2);

		for (unsigned i = 0; i < numInstructions; i++)
		{
			if (elems_.size() > 1)
			{
				unsigned idx = Random::unsignedRange(1, elems_.size() - 1);

				if (isCell(idx))
					elems_.erase(elems_.begin() + idx);
			}
		}

		return;
	}

	unsigned cellSize = 0;
	for (unsigned i = 0; i < elems_.size(); i++)
	{
		if (isCell(i))
			cellSize++;
	}

	if (isMover_)
	{
		if (cellSize >= 5)
		{
			if (isHerbivore_)
			{

				for (auto& elem : elems_)
					if (elem == 'O')
						elem = 'C';
			}
			else if (isCarnivore_)
			{
				for (auto& elem : elems_)
					if (elem == 'C')
						elem = 'O';
			}

			return;
		}
	}
	else
	{
		if (cellSize >= 5)
			return;
	}

	unsigned numMaxMutations = 5 - cellSize;

	unsigned idx = getRandomCellIdx();
	char cellType = elems_[idx];
	std::vector<char> productionV;

	switch (cellType)
	{
	case 'P':
		productionV = Production::plant();
		break;
	case 'O':
		productionV = Production::mouth(numMaxMutations);
		break;
	case 'M':
		productionV = Production::mover(numMaxMutations);
		break;
	case 'T':
		productionV = Production::thorn();
		break;
	case 'C':
		productionV = Production::carnivore(numMaxMutations);
		break;
	}

	DNA production(productionV);

	for (int i = production.size() - 1; i >= 0; i--)
		elems_.insert(elems_.begin() + idx + 1, production.get(i));

	bool isCarnivore = std::ranges::find(elems_, 'C') != std::end(elems_);
	bool isHerbivore = std::ranges::find(elems_, 'O') != std::end(elems_);

	if (isCarnivore && isHerbivore)
	{
		if (Random::unsignedRange(0, 1) == 0)
		{
			for (auto& elem : elems_)
				if (elem == 'O')
					elem = 'C';
		}
		else
		{
			for (auto& elem : elems_)
				if (elem == 'C')
					elem = 'O';
		}
	}

	str_ = get();
}

void DNA::setString(const std::string& str)
{
	str_ = str;
	isMover_ = false;
	elems_.clear();

	for (unsigned i = 0; i < str.size(); i++)
	{
		if (str[i] == 'M')
			isMover_ = true;

		elems_.push_back(str[i]);
	}
}

bool DNA::isMover()
{
	return isMover_;
}

unsigned DNA::size()
{
	return elems_.size();
}

char DNA::get(unsigned idx)
{
	return elems_[idx];
}

std::string DNA::str()
{
	return str_;
}

bool DNA::isCell(unsigned idx)
{
	return elems_[idx] == 'M' || elems_[idx] == 'T' || elems_[idx] == 'O' || elems_[idx] == 'P' || elems_[idx] == 'C';
}

bool DNA::hasCell()
{
	for (unsigned i = 0; i < elems_.size(); i++)
		if (isCell(i))
			return true;

	return false;
}

std::string DNA::get() const
{
	return std::string(elems_.begin(), elems_.end());
}

bool DNA::operator==(const DNA& other)
{
	return other.str_ == str_;
}

unsigned DNA::getRandomCellIdx()
{
	unsigned idx = Random::unsignedRange(0, elems_.size() - 1);

	while (!isCell(idx))
		idx = Random::unsignedRange(0, elems_.size() - 1);

	return idx;
}
