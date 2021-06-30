#pragma once
#include <random>
class RandGen
{
public:
	static float GenRandomFloat(float min, float max)
	{
		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_real_distribution<> distr(min, max);
		return static_cast<float>(distr(eng));
	}
	static float GenRandomFloat(float min, float max, unsigned int seed)
	{
		std::mt19937 eng(seed);
		std::uniform_real_distribution<> distr(min, max);
		return static_cast<float>(distr(eng));
	}
	static int GenRandomInt(int min, int max)
	{
		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_int_distribution<> distr(min, max);
		return distr(eng);
	}
	static int GenRandomInt(int min, int max, unsigned int seed)
	{
		std::mt19937 eng(seed);
		std::uniform_int_distribution<> distr(min, max);
		return distr(eng);
	}
};