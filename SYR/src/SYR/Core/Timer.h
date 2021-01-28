#pragma once

#include <chrono>

namespace SYR {
	class Timer {
	public:
		Timer(const char* name);
		~Timer();

		void stop();

	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		bool m_Stopped;
	};
}