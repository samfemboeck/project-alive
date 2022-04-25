#include "../pch.h"
#include "TickCountTimer.h"
 

TickCountTimer::TickCountTimer(long interval_ms) : ITimerBase(interval_ms)
{
	m_start = std::chrono::high_resolution_clock::now();
}

long TickCountTimer::get_current_time_ms()
{
	return m_elapsed_ms;
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
