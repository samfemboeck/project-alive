module;
#include <time.h>
#include <chrono>
export module Timer;

export class ITimerBase
{
public:
	ITimerBase(long interval_ms) :
		m_interval_ms(interval_ms)
	{
	}

	virtual long get_current_time_ms() = 0; // return current time in ms
	virtual bool update() = 0; // return true if the interval has elapsed

protected:
	long m_interval_ms;  // holds interval in milliseconds
};

export class ClockTimer final : public ITimerBase
{
public:
	ClockTimer(long interval_ms) : ITimerBase(interval_ms)
	{
		m_start = clock();
	}

	long get_current_time_ms()
	{
		return m_elapsed;
	}

	bool update()
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

private:
	clock_t m_elapsed = 0;
	clock_t m_start;
};

export class TickCountTimer final : public ITimerBase
{
public:
	TickCountTimer(long interval_ms) : ITimerBase(interval_ms)
	{
		m_start = std::chrono::high_resolution_clock::now();
	}

	long get_current_time_ms()
	{
		return m_elapsed_ms;
	}

	bool update()
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
private:
	long m_elapsed_ms = 0;
	std::chrono::high_resolution_clock::time_point m_start;
};
