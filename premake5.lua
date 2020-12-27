-- -------------------------------------------------
-- -------------------------------------------------
-- --------- Saffron 2D Project Generator ----------
-- -------------------------------------------------
-- -------------------------------------------------


-- --------------------------------------
-- Saffron Workspace
-- --------------------------------------

workspace "Saffron"
	architecture "x64"
	
	configurations 
	{ 
		"Debug", 
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	startproject "Project"
	


-- VARIABLES
SfmlLibs = "../Deps/SFML/lib"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Saffron"] = "Saffron/Src"
IncludeDir["entt"] = "Deps/entt/include"
IncludeDir["ImGui"] = "Deps/ImGui"
IncludeDir["SFML"] = "Deps/SFML/include"
IncludeDir["spdlog"] = "Deps/spdlog/include"

LibraryDir = {}

SharedLibraryDir = {}
SharedLibraryDir["SFML"] = SfmlLibs .. "/openal32.dll"

RuntimeLibararyDir = {}

outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- --------------------------------------
-- Engine
-- --------------------------------------

group "Engine"

group "Engine/Dependencies"
	include "Deps/entt/premake5"
	include "Deps/ImGui/premake5"
	include "Deps/SFML/premake5"
	include "Deps/spdlog/premake5"
group "Engine"

-- --------------------------------------
-- Saffron
-- --------------------------------------

project "Saffron"
	location "Saffron"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputDirectory .. "/%{prj.name}")

	pchheader "SaffronPCH.h"
	pchsource "Saffron/Src/SaffronPCH.cpp"

	files 
	{ 
		"%{prj.name}/Src/**.h", 
		"%{prj.name}/Src/**.c", 
		"%{prj.name}/Src/**.hpp", 
		"%{prj.name}/Src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Src",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.SFML}",
		"%{IncludeDir.spdlog}"
	}
	
	defines
	{
		"SFML_STATIC"
	}
	
	links 
	{
		"SFML",
		"ImGui",
		"opengl32.lib",
	}

	filter "system:windows"
		systemversion "latest"
		
		defines 
		{ 
			"_CRT_SECURE_NO_WARNINGS",			
			"SE_PLATFORM_WINDOWS"
		}
		
	filter "configurations:Debug"
		defines "SE_DEBUG"
		symbols "On"
			
	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"
		
group ""


-- --------------------------------------
-- Project
-- --------------------------------------
		
project "Project"
	location "Project"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir ("Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputDirectory .. "/%{prj.name}")
	
	files 
	{ 
		"%{prj.name}/Src/**.h", 
		"%{prj.name}/Src/**.c", 
		"%{prj.name}/Src/**.hpp", 
		"%{prj.name}/Src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Src",
		"Saffron/Src",
		"%{IncludeDir.Saffron}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.SFML}",
		"%{IncludeDir.spdlog}",
	}
	
	defines
	{
		"SFML_STATIC"
	}

	links 
	{			
		"SFML",
		"Saffron",
		"ImGui",
	}
	
	postbuildcommands 
	{
		'{COPY} "../Resources/" "%{cfg.targetdir}/Resources/"',
		'{COPY} "../Resources/" "Resources/"'
	}
	
	filter "system:windows"
		systemversion "latest"
		
		defines 
		{ 
			"_CRT_SECURE_NO_WARNINGS",			
			"SE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "SE_DEBUG"
		symbols "On"
		
		postbuildcommands 
		{
			'{COPY} "%{SharedLibraryDir.SFML}" "%{cfg.targetdir}"'
		}
			
	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "On"
		
		postbuildcommands 
		{
			'{COPY} "%{SharedLibraryDir.SFML}" "%{cfg.targetdir}"'
		}

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"
		
		postbuildcommands 
		{
			'{COPY} "%{SharedLibraryDir.SFML}" "%{cfg.targetdir}"'
		}