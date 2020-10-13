include("conanbuildinfo.premake.lua")

workspace "Pubsearch"
	conan_basic_setup()

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

project "Pubsearch"
	location "./"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/")

	files
	{
	"src/**.hpp",
	"src/**.cpp"
	}


    filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"

    filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"
