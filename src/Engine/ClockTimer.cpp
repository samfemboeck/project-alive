#include "../pch.h"
#include "ClockTimer.h"

ClockTimer::ClockTimer(long interval_ms) : ITimerBase(interval_ms)
{
	m_start = clock();
}

long ClockTimer::get_current_time_ms()
{
	return m_elapsed;
}

bool ClockTimer::update()
{
	auto now = clock();
	m_elapsed += (now - m_start) / CLOCKS_PER_SEC * 1000;
	if (m_elapsed >= m_interval_ms)
	{
		m_elapsed -= m_interval_ms;
		m_start = now;
		return true;
	}

	return false;
}
