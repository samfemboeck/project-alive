#pragma once

#include <string>

class DNA
{
public:
	inline static bool AllowThorns = false;

public:
	DNA() = default;
	DNA(const std::vector<char>& elems);
	DNA(std::string dna);
	void mutate();
	void setString(const std::string& str);
	bool isMover();
	unsigned size();
	char get(unsigned idx);
	std::string str();
	bool isCell(unsigned idx);
	bool hasCell();
	std::string get() const;
	bool operator==(const DNA& other);

private:
	bool isMover_ = false;
	bool isHerbivore_ = false;
	bool isCarnivore_ = false;
	unsigned getRandomCellIdx();
	std::vector<char> elems_;
	std::string str_;
	unsigned numCells_ = 0;
};
