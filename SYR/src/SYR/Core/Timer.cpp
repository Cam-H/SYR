#include "syrpch.h"
#include "Timer.h"

namespace SYR {

	Timer::Timer(const char* name) : m_Name(name), m_Stopped(false) {
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	Timer::~Timer() {
		if (!m_Stopped) {
			stop();
		}
	}

	void Timer::stop() {
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		float duration = (end - start) * 0.001f;

		m_Stopped = true;

		std::cout << "Duration: " << duration << "ms" << std::endl;
	}
}