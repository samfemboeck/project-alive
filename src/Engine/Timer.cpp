#include "../pch.h"
#include "Timer.h"

ClockTimer::ClockTimer(long interval_ms)
{
	m_start = clock();
}

bool ClockTimer::update()
{
	auto now = clock();
	m_elapsed_ms += (now - m_start) / CLOCKS_PER_SEC * 1000;
	if (m_elapsed_ms >= m_interval_ms)
	{
		m_elapsed_ms -= m_interval_ms;
		m_start = now;
		return true;
	}

	return false;
}

TickCountTimer::TickCountTimer(long interval_ms) :
	m_interval_ms(interval_ms),
	m_start(std::chrono::high_resolution_clock::now())
{
}

void TickCountTimer::add_time(long time)
{
	m_interval_ms += time;
}

bool TickCountTimer::update()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start).count();
	m_start = now;
	m_elapsed_ms += diff;
	if (m_elapsed_ms >= m_interval_ms)
	{
		m_elapsed_ms -= m_interval_ms;
		return true;
	}

	return false;
}

long TickCountTimer::get_elapsed_ms()
{
	return m_elapsed_ms;
}

long TickCountTimer::get_interval_ms()
{
	return m_interval_ms;
}

ScopeTimer::ScopeTimer(const std::string& method_name) :
	m_method_name(method_name)
{
	m_start = std::chrono::high_resolution_clock::now();
}

ScopeTimer::~ScopeTimer()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start).count();
	LOG("{}: {} ms", m_method_name, elapsed);
}
