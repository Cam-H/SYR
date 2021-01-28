#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace SYR {

	class SYR_API Log{

	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

//Core log macros
#define SYR_CORE_FATAL(...)   ::SYR::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define SYR_CORE_ERROR(...)   ::SYR::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SYR_CORE_WARN(...)    ::SYR::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SYR_CORE_INFO(...)    ::SYR::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SYR_CORE_TRACE(...)   ::SYR::Log::GetCoreLogger()->trace(__VA_ARGS__)

#define SYR_CORE_PROFILE(...)

//Client log macros
#define SYR_FATAL(...)   ::SYR::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define SYR_ERROR(...)   ::SYR::Log::GetClientLogger()->error(__VA_ARGS__)
#define SYR_WARN(...)    ::SYR::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SYR_INFO(...)    ::SYR::Log::GetClientLogger()->info(__VA_ARGS__)
#define SYR_TRACE(...)   ::SYR::Log::GetClientLogger()->trace(__VA_ARGS__)

#define SYR_PROFILE(...)


