#pragma once

#include "ITimerBase.h"

class ClockTimer final : public ITimerBase
{
public:
	ClockTimer(long interval_ms);
	long get_current_time_ms() override;
	bool update() override;
private:
	clock_t m_elapsed = 0;
	clock_t m_start;
};