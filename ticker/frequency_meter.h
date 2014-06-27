#pragma once

#include <random>
#include <mutex>

class FrequencyMeter
{
	mutable std::mt19937 gen;
	mutable std::uniform_int_distribution<int> dis;
	mutable std::mutex m;
public:
	FrequencyMeter() :
		dis(0, 3),
		gen(std::random_device()())
	{}

	inline int Hz() const
	{
		std::lock_guard<std::mutex> lock(m);
		return 60 + dis(gen);
	}
};
