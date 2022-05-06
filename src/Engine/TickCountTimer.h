#pragma once

#include "ITimerBase.h"
#include <chrono>

class TickCountTimer final : public ITimerBase
{
public:
	TickCountTimer(long interval_ms);
	long get_current_time_ms() override;
	virtual bool update() override;
private:
	long m_elapsed_ms = 0;
	std::chrono::high_resolution_clock::time_point m_start;
};