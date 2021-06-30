#pragma once

enum class Duration
{
	SECONDS,
	MILLISECONDS,
	MICROSECONDS
};

class Timer
{
public:
	Timer(Duration duration = Duration::MICROSECONDS);
	~Timer() = default;


	

	void start();
	uint32_t stop();
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start, m_stop;
	Duration m_duration;
};

