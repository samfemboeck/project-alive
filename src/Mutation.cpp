#include "pch.h"
#include "Mutation.h"
#include "Engine/QuickMaths.h"

std::vector<std::string> Production::mover()
{
	std::vector<std::string> vocabulary = { "M", "LM", "RM", "[LM]","[LM]","[RM]","[RM]","[LM]","[LM]","[RM]","[RM]", "M", "LM", "RM", "[LM]", "[RM]", "[LM]", "[RM]", "O", "LO", "RO", "[LO]", "[RO]"};
	std::vector<std::string> ret;
	unsigned numInstructions = Random::unsignedRange(1, 1);

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = Random::unsignedRange(0, vocabulary.size() - 1);
		ret.push_back(vocabulary[idx]);
	}

	return ret;
}

std::vector<std::string> Production::plant()
{
	std::vector<std::string> vocabulary = { "P", "LP", "RP", "[LP]", "[RP]","[LP]", "[RP]" };
	std::vector<std::string> ret;
	unsigned numInstructions = Random::unsignedRange(1, 1);

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = Random::unsignedRange(0, vocabulary.size() - 1);
		ret.push_back(vocabulary[idx]);
	}

	return ret;
}

std::vector<std::string> Production::mouth()
{	
	std::vector<std::string> vocabulary = { "O", "LO", "RO", "[LO]", "[RO]", "[LO]", "[RO]","[LO]", "[RO]", "[LO]", "[RO]", "O", "LO", "RO", "[LO]", "[RO]", "[LO]", "[RO]", "M", "LM", "RM", "[LM]", "[RM]", "[LM]", "[RM]"};
	std::vector<std::string> ret;
	unsigned numInstructions = Random::unsignedRange(1, 1);

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = Random::unsignedRange(0, vocabulary.size() - 1);
		ret.push_back(vocabulary[idx]);
	}

	return ret;
}

std::vector<std::string> Production::thorn()
{	
	std::vector<std::string> vocabulary = { "T", "LT", "RT", "[LT]", "[RT]", "[LT]", "[RT]"};
	std::vector<std::string> ret;
	unsigned numInstructions = Random::unsignedRange(1, 1);

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = Random::unsignedRange(0, vocabulary.size() - 1);
		ret.push_back(vocabulary[idx]);
	}

	return ret;
}

DNA::DNA(std::vector<std::string> elems) :
	elems_(elems)
{
	for (auto& elem : elems)
		if (elem[0] == 'M')
			isMover_ = true;
}

DNA::DNA(std::string dna)
{
	setString(dna);
}

void DNA::pushBack(std::string elem)
{
	elems_.push_back(elem);
}

void DNA::insertRandom(std::string elem)
{
	elems_.insert(elems_.begin() + Random::unsignedRange(0, elems_.size() - 1), elem);
}

void DNA::eraseRandom()
{
	elems_.erase(elems_.begin() + getRandomCellIdx());
}

std::string DNA::get()
{
	std::string ret;

	for (const std::string& elem : elems_)
		ret += elem;

	return ret;
}

void DNA::mutate()
{		
	if (!isMover() && Random::unsignedRange(0, 50) == 0)
	{
		elems_.clear();
		elems_.push_back("O");
		return;
	}

	unsigned idx = getRandomCellIdx();
	char cellType = elems_[idx][0];
	std::vector<std::string> productionV;

	switch (cellType)
	{
	case 'P':
		productionV = Production::plant();
		break;
	case 'O':
		productionV = Production::mouth();
		break;
	case 'M':
		productionV = Production::mover();
		break;
	case 'T':
		productionV = Production::thorn();
		break;
	}

	DNA production(productionV);

	for (int i = production.size() - 1; i >= 0; i--)
		elems_.insert(elems_.begin() + idx + 1, production.get(i));
}

void DNA::setString(const std::string& str)
{
	isMover_ = false;
	elems_.clear();

	for (unsigned i = 0; i < str.size(); i++)
	{
		if (str[i] == 'M')
			isMover_ = true;

		elems_.push_back(std::string(1, str[i]));
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

std::string DNA::get(unsigned idx)
{
	return elems_[idx];
}

bool DNA::isCell(unsigned idx)
{
	return elems_[idx][0] == 'M' || elems_[idx][0] == 'T' || elems_[idx][0] == 'O' || elems_[idx][0] == 'P';
}

bool DNA::hasCell()
{
	for (unsigned i = 0; i < elems_.size(); i++)
		if (isCell(i))
			return true;

	return false;
}

unsigned DNA::getRandomCellIdx()
{
	unsigned idx = Random::unsignedRange(0, elems_.size() - 1);

	while (!isCell(idx))
		idx = Random::unsignedRange(0, elems_.size() - 1);

	return idx;
}
