workspace "SYR"
	architecture "x64"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)

IncludeDir = {}
IncludeDir["GLFW"] = "SYR/vendor/GLFW/include"
IncludeDir["Glad"] = "SYR/vendor/Glad/include"
IncludeDir["glm"] = "SYR/vendor/glm"
IncludeDir["stb"] = "SYR/vendor/stb"
IncludeDir["entt"] = "SYR/vendor/entt/include"
IncludeDir["freetype"] = "SYR/vendor/freetype/include"

include "SYR/vendor/GLFW"
include "SYR/vendor/Glad"
include "SYR/vendor/freetype"

project "SYR"
	location "SYR"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/" .. outputdir .. "/%{prj.name}")

	pchheader "syrpch.h"
	pchsource "SYR/src/syrpch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb/**.h",
		"%{prj.name}/vendor/stb/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.h**",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/entt/include/**.h**"
	}

	includedirs{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.freetype}"
	}

	links{
		"GLFW",
		"Glad",
		"freetype"
	}

	filter "system:windows"
		staticruntime "On"
		systemversion "latest"

		defines{
			"SYR_PLATFORM_WINDOWS",
			"SYR_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "SYR_DEBUG"
		buildoptions "/MDd"
		symbols "on"

	filter "configurations:Release"
		defines "SYR_RELEASE"
		buildoptions "/MD"
		optimize "on"

	filter "configurations:Dist"
		defines "SYR_DIST"
		buildoptions "/MD"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"SYR/vendor/spdlog/include",
		"SYR/src",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}

	links{
		"SYR"
	}

	filter "system:windows"
		systemversion "latest"

		defines{
			"SYR_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "SYR_DEBUG"
		buildoptions "/MDd"
		symbols "on"

	filter "configurations:Release"
		defines "SYR_RELEASE"
		buildoptions "/MD"
		optimize "on"

	filter "configurations:Dist"
		defines "SYR_DIST"
		buildoptions "/MD"
		optimize "on"