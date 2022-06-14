#pragma once

#include <string>

class DNA
{
public:
	DNA() = default;
	DNA(std::vector<std::string> elems);
	DNA(std::string dna);
	void pushBack(std::string elem);
	void insertRandom(std::string elem);
	void eraseRandom();
	std::string get();
	void mutate();
	void setString(const std::string& str);
	bool isMover();
	unsigned size();
	std::string get(unsigned idx);

private:
	bool isMover_ = false;
	unsigned getRandomCellIdx();
	std::vector<std::string> elems_;
};

class Production
{
public:
	static std::vector<std::string> mover();
	static std::vector<std::string> plant();
	static std::vector<std::string> mouth();
	static std::vector<std::string> thorn();
};
