-- premake5.lua
workspace "RayTracing"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "RayTracing"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

project "Tests"
	location "tests"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	targetdir "bin/%{cfg.buildcfg}"
	staticruntime "off"
	
	files 
	{
		"tests/src/**.cpp",
		"tests/include/**.h",
	}
	
	includedirs
	{
		"../Walnut/vendor/imgui",
		"../Walnut/vendor/glfw/include",
		"../Walnut/vendor/glm",
		
		"../Walnut/Walnut/src",
		
		"%{IncludeDir.VulkanSDK}",
		
		"RayTracing/include/",
	}
	
	links
	{
		"RayTracing",
	}
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
include "RayTracing"