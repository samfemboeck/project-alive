#include "pch.h"
#include "Mutation.h"
#include "Engine/QuickMaths.h"

std::string Production::mutate(std::string production)
{
	std::vector<char> cellTypes = { 'P', 'O', 'M', 'T' };
	std::random_device rd;
    std::mt19937 gen{rd()};
	std::ranges::shuffle(cellTypes, gen);
	unsigned cellIdx = 0;
	size_t idx = production.find(cellTypes[cellIdx]);
	while (idx == std::string::npos)
		idx = production.find(cellTypes[++cellIdx]);

	unsigned param = production[idx + 2] - '0';
	unsigned newParam = param;
	while (newParam == param)
		newParam = Random::unsignedRange(0, 5);

	production[idx + 2] = '0' + newParam;
	return production;
}

std::string Production::produce(std::string predecessor)
{
	std::vector<char> cellTypes = { 'P', 'O', 'M', 'T' };
	std::random_device rd;
    std::mt19937 gen{rd()};
	std::ranges::shuffle(cellTypes, gen);
	unsigned cellIdx = 0;
	size_t idx = predecessor.find(cellTypes[cellIdx]);
	while (idx == std::string::npos)
		idx = predecessor.find(cellTypes[++cellIdx]);

	std::function<std::string(unsigned)> prod;	
	
	switch (predecessor[idx])
	{
	case 'P':
		prod = Production::plant;
		break;
	case 'O':
		prod = Production::mouth;
		break;
	case 'M':
		prod = Production::mover;
		break;
	case 'T':
		prod = Production::thorn;
	}

	while (idx != std::string::npos)
	{
		unsigned arg = predecessor[idx + 2] - '0';
		std::string production = prod(arg);
		predecessor = predecessor.substr(0, idx) + production + predecessor.substr(idx + 4);
		idx = predecessor.find(cellTypes, idx);
	}

	return predecessor;
}

std::string Production::mover(unsigned arg)
{
	switch (arg)
	{
	case 0:
		return "M(0)[T(0)F(1,0)]";
	case 1:
		return "M(1)[T(0)L(1,1)F(1,1)]";
	case 2:
		return "M(2)[O(2)F(1,2)]";
	case 3:
		return "M(3)[T(3)F(1,3)]";
	case 4:
		return "M(4)[O(4)F(1,4)]";
	case 5:
		return "M(5)[P(5)F(1,5)]";
	}
}

std::string Production::plant(unsigned arg)
{
	switch (arg)
	{
	case 0:
		return "P(0)[L(1,0)F(1,0)]";
	case 1:
		return "P(1)[R(1,1)F(1,1)]";
	case 2:
		return "P(2)[F(1,2)]";
	case 3:
		return "T(3)";
	case 4:
		return "P(4)[T(4)R(1,4)F(1,4)]";
	case 5:
		return "P(5)[T(4)L(1,5)F(1,5)]";
	}
}

std::string Production::mouth(unsigned arg)
{
	switch (arg)
	{
	case 0:
		return "O(0)[L(1,0)F(1,0)]";
	case 1:
		return "O(1)[R(1,1)F(1,1)]";
	case 2:
		return "T(2)[O(2)F(1,2)]";
	case 3:
		return "T(2)[O(2)R(1,1)F(1,2)]";
	case 4:
		return "P(4)";
	case 5:
		return "M(5)";
	}
}

std::string Production::thorn(unsigned arg)
{
	switch (arg)
	{
	case 0:
		return "T(0)[L(1,0)F(1,0)]";
	case 1:
		return "T(1)[R(1,1)F(1,1)]";
	case 2:
		return "O(2)[T(2)F(1,2)]";
	case 3:
		return "O(2)[T(2)R(1,1)F(1,2)]";
	case 4:
		return "O(2)[T(2)L(1,1)F(1,2)]";
	case 5:
		return "P(5)";
	}
}
