workspace "DNA_Replication"
	configurations { "Debug", "Release" }

project "dnarepl"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++20"
	targetdir "."

	files { "src/**.hpp", "src/**.cpp" }
	includedirs { "src", "/usr/include/freetype2" }
	links { "SDL2", "GL", "GLEW", "freetype"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		debugger "GDB"
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"