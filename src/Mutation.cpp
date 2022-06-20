#include "pch.h"
#include "Mutation.h"
#include "Engine/QuickMaths.h"

std::vector<char> Production::mover()
{
	//std::vector<std::string> vocabulary = { "LM", "RM", "[LM]","[LM]","[RM]","[RM]","[LM]","[LM]","[RM]","[RM]","LM", "RM", "[LM]", "[RM]", "[LM]", "[RM]", "O", "LO", "RO", "[LO]", "[RO]", "T", "LT", "RT", "[T]", "[LT]", "[RT]", "T", "LT", "RT", "[T]", "[LT]", "[RT]"};
	std::vector<std::string> vocabulary = { "M", "LM", "RM", "[M]", "[LM]","[RM]","O", "LO", "RO", "[O]", "[LO]", "[RO]", "T", "LT", "RT", "[T]", "[LT]", "[RT]"};
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

std::vector<char> Production::plant()
{
	std::vector<std::string> vocabulary = { "P", "LP", "RP", "[P]", "[LP]", "[RP]","P", "LP", "RP", "[P]", "[LP]", "[RP]","[LP]", "[RP]", "T", "LT", "RT", "[T]", "[LT]", "[RT]" };
	//std::vector<std::string> vocabulary = { "P", "LP", "RP", "[LP]", "[RP]","[LP]", "[RP]","P", "LP", "RP", "[LP]", "[RP]","[LP]", "[RP]"};
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

std::vector<char> Production::mouth()
{	
	//std::vector<std::string> vocabulary = { "O", "LO", "RO", "[LO]", "[RO]", "[LO]", "[RO]","[LO]", "[RO]", "[LO]", "[RO]", "O", "LO", "RO", "[LO]", "[RO]", "[LO]", "[RO]", "M", "LM", "RM", "[LM]", "[RM]", "[LM]", "[RM]", "T", "LT", "RT", "[T]", "[LT]", "[RT]", "T", "LT", "RT", "[T]", "[LT]", "[RT]"};
	std::vector<std::string> vocabulary = { "O", "LO", "RO", "[O]", "[LO]", "[RO]", "M", "LM", "RM", "[M]", "[LM]", "[RM]", "T", "LT", "RT", "[T]", "[LT]", "[RT]"};
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

std::vector<char> Production::thorn()
{	
	std::vector<std::string> vocabulary = { "T", "LT", "RT", "[T]", "[LT]", "[RT]"};
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

DNA::DNA(const std::vector<char>& elems) :
	elems_(elems),
	str_(std::string(elems))
{
	for (char c : elems)
		if (c == 'M')
			isMover_ = true;
}

DNA::DNA(std::string dna)
{
	for (char c : dna)
	{
		elems_.push_back(c);
		if (c == 'M')
			isMover_ = true;
	}

	str_ = std::string(elems_);
}

void DNA::mutate()
{		
	if (Random::unsignedRange(0, 1) == 0)
	{
		unsigned idx = Random::unsignedRange(0, elems_.size() - 1);

		if (isCell(idx))
			elems_.erase(elems_.begin() + idx);

		return;
	}

	unsigned idx = getRandomCellIdx();
	char cellType = elems_[idx];
	std::vector<char> productionV;

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

	bool isPredator = std::ranges::find(elems_, 'T') != std::end(elems_);
	bool isMover = std::ranges::find(elems_, 'M') != std::end(elems_);
	bool isHerbivore = std::ranges::find(elems_, 'O') != std::end(elems_);

	if (isPredator && isMover && isHerbivore)
	{
		for (auto& elem : elems_)
			if (elem == 'O')
				elem = 'T';
	}
}

void DNA::setString(const std::string& str)
{
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
	return elems_[idx] == 'M' || elems_[idx] == 'T' || elems_[idx] == 'O' || elems_[idx] == 'P';
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
	return std::string(elems_);
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
