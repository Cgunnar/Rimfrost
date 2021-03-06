#include "rfpch.hpp"
#include "Timer.hpp"

using namespace std;
using namespace chrono;

Timer::Timer(Duration duration) : m_duration(duration), m_start(high_resolution_clock::now()), m_stop(high_resolution_clock::now())
{

}

void Timer::start()
{
	m_start = high_resolution_clock::now();
}

uint32_t Timer::stop()
{
	m_stop = high_resolution_clock::now();
	switch (m_duration)
	{
	case Duration::SECONDS:
		return static_cast<uint32_t>(duration_cast<seconds>(m_stop - m_start).count());
	case Duration::MILLISECONDS:
		return static_cast<uint32_t>(duration_cast<milliseconds>(m_stop - m_start).count());
	case Duration::MICROSECONDS:
		return static_cast<uint32_t>(duration_cast<microseconds>(m_stop - m_start).count());
	default:
		return -1;
	}
}
