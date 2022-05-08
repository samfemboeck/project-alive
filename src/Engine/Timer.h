#pragma once

#include <chrono>

class ClockTimer
{
public:
	ClockTimer(long interval_ms);
	bool update();
private:
	clock_t m_elapsed_ms = 0;
	clock_t m_start;
	long m_interval_ms;
};

class TickCountTimer
{
public:
	TickCountTimer(long interval_ms);
	bool update();
	void add_time(long time_ms);
	long get_elapsed_ms();
	long get_interval_ms();
private:
	long m_interval_ms;
	long m_elapsed_ms = 0;
	std::chrono::high_resolution_clock::time_point m_start;
};

class ScopeTimer
{
public:
	ScopeTimer(const std::string& method_name);
	~ScopeTimer();
private:
	std::string m_method_name;
	std::chrono::high_resolution_clock::time_point m_start;
};
