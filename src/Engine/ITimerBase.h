#pragma once

class ITimerBase
{
public:
ITimerBase(long interval_ms): 
	m_interval_ms(interval_ms) 
{
}

virtual long get_current_time_ms() = 0; // return current time in ms
virtual bool update() = 0; // return true if the interval has elapsed

protected:
long m_interval_ms;  // holds interval in milliseconds
};
