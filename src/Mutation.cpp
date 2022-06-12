#include "pch.h"
#include "Mutation.h"
#include "Engine/QuickMaths.h"

std::vector<std::string> Production::mover(unsigned arg)
{
	std::string vocabulary = "MLRTO";
	std::vector<std::string> ret;
	unsigned numInstructions = rand() % 3 + 1;

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = rand() % vocabulary.size();
		std::string str;
		str += vocabulary[idx];
		str += "(0)";
		ret.push_back(str);
	}

	return ret;
}

std::vector<std::string> Production::plant(unsigned arg)
{	
	std::string vocabulary = "PLR";
	std::vector<std::string> ret;
	unsigned numInstructions = rand() % 3 + 1;

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = rand() % vocabulary.size();
		std::string str;
		str += vocabulary[idx];
		str += "(0)";
		ret.push_back(str);
	}

	return ret;
}

std::vector<std::string> Production::mouth(unsigned arg)
{	
	std::string vocabulary = "OLRTM";
	std::vector<std::string> ret;
	unsigned numInstructions = rand() % 3 + 1;

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = rand() % vocabulary.size();
		std::string str;
		str += vocabulary[idx];
		str += "(0)";
		ret.push_back(str);
	}

	return ret;
}

std::vector<std::string> Production::thorn(unsigned arg)
{	
	std::string vocabulary = "OLRTM";
	std::vector<std::string> ret;
	unsigned numInstructions = rand() % 3 + 1;

	for (unsigned i = 0; i < numInstructions; i++)
	{
		unsigned idx = rand() % vocabulary.size();
		std::string str;
		str += vocabulary[idx];
		str += "(0)";
		ret.push_back(str);
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
	elems_.insert(elems_.begin() + (rand() % elems_.size()), elem);
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

void DNA::mutateArg()
{
	unsigned idx = getRandomCellIdx();
	elems_[idx][2] = (rand() % 6) + '0';
}

void DNA::mutate()
{	
	unsigned idx = getRandomCellIdx();
	unsigned arg = elems_[idx][2] - '0';
	char cellType = elems_[idx][0];
	std::vector<std::string> productionV;

	switch (cellType)
	{
	case 'P':
		productionV = Production::plant(arg);
		break;
	case 'O':
		productionV = Production::mouth(arg);
		break;
	case 'M':
		productionV = Production::mover(arg);
		break;
	case 'T':
		productionV = Production::thorn(arg);
		break;
	}

	elems_.erase(elems_.begin() + idx);

	DNA production(productionV);

	for (int i = production.size() - 1; i >= 0; i--)
		elems_.insert(elems_.begin() + idx, production.get(i));
}

void DNA::setString(const std::string& str)
{
	isMover_ = false;
	elems_.clear();

	for (unsigned i = 0; i < str.size();)
	{
		if (isalpha(str[i]))
		{
			if (str[i] == 'M')
				isMover_ = true;

			elems_.push_back(str.substr(i, 4));
			i += 4;
		}
		else
		{
			elems_.push_back("" + str[i]);
			i += 1;
		}
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

unsigned DNA::getRandomCellIdx()
{
	unsigned idx = rand() % elems_.size();
	while (elems_[idx][0] != 'M' && elems_[idx][0] != 'T' && elems_[idx][0] != 'O' && elems_[idx][0] != 'P')
		idx = rand() % elems_.size();
	return idx;
}
