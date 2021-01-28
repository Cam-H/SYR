#pragma once

#include <memory>

#ifdef SYR_PLATFORM_WINDOWS
	#ifdef SYR_DYANMIC_LINK
		#ifdef SYR_BUILD_DLL
			#define SYR_API __declspec(dllexport)
		#else
			#define SYR_API __declspec(dllimport)
		#endif
	#else
		#define SYR_API
	#endif
#else
	#error SYR only supports Windows!
#endif

#ifdef SYR_ENABLE_ASSERTS
	#define SYR_ASSERT(x, ...) { if(!(x)){SYR_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define SYR_CORE_ASSERT(x, ...){if(!(x)){SYR_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define SYR_ASSERT(x, ...)
	#define SYR_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define SYR_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
#define SYR_BIND_EVENT_STATIC_FN(fn) std::bind(fn, std::placeholders::_1);

namespace SYR {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> createScope(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> createRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}