#pragma once

#include <string>

class Production
{
public:
	static std::string mutate(std::string production);
	static std::string produce(std::string precessor);
	static std::string mover(unsigned arg);
	static std::string plant(unsigned arg);
	static std::string mouth(unsigned arg);
	static std::string thorn(unsigned arg);
};
