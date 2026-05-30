---@diagnostic disable: undefined-global

local ProjectName = "fractals"

local function GetBasePath()
	return debug.getinfo(1).source:match("@?(.*/)")
end

workspace "saffron"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	flags {
		"MultiProcessorCompile"
	}

    location (WrkLoc)

	startproject (ProjectName)

local saffronEngine2D = dofile("deps/saffron-engine-2d/premake5.lua")

project (ProjectName)
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
	staticruntime "On"

	filter "system:linux"
		buildoptions { "-std=c++23" }
	filter "system:windows"
		buildoptions { "/std:c++latest" }
	filter {}

	-- The CPU fractal kernels (via Saffron's SIMD.h) use AVX2 intrinsics.
	-- MSVC exposes these unconditionally; gcc/clang need the ISA enabled.
	vectorextensions "AVX2"

	objdir (OutObj)
	location (OutLoc)

    filter "configurations:Debug or Release"
	    targetdir (OutBin)
        saffronEngine2D.PreBuild("Debug", OutBin, PrjLoc)
        saffronEngine2D.PostBuild("Debug", OutBin, PrjLoc)

        saffronEngine2D.PreBuild("Release", OutBin, PrjLoc)
        saffronEngine2D.PostBuild("Release", OutBin, PrjLoc)

    filter "configurations:Dist"
        targetdir (OutBinDist)
        saffronEngine2D.PreBuild("Dist", OutBinDist, PrjLoc)
        saffronEngine2D.PostBuild("Dist", OutBinDist, PrjLoc)

    filter "configurations:Debug or Release or Dist"

    files {
        "src/**.h",
		"src/**.c",
		"src/**.hpp",
		"src/**.cpp",
		"assets/shaders/**.*",
    }

    vpaths {
        ["Shaders"] = { "assets/shaders/**.*" }
    }

    includedirs {
        "src",
    }
    
    saffronEngine2D.Include()
    saffronEngine2D.Link()
    saffronEngine2D.AddDefines()

    -- premake propagates the engine static lib to the executable but not the
    -- engine's external (-l) dependencies, so the final link needs them here.
    -- On Windows these come from the vendored prebuilts via the engine project.
    filter "system:linux"
        libdirs {
            "deps/saffron-engine-2d/deps/Box2D/lib/linux",
            "deps/saffron-engine-2d/deps/Glad/lib/linux",
        }
        links {
            "ImGui",
            "sfml-graphics",
            "sfml-window",
            "sfml-audio",
            "sfml-network",
            "sfml-system",
            "Box2D",
            "Glad",
            "GL",
            "dl",
            "pthread",
        }
    filter {}

    local from = GetBasePath() .. AstFol
    CopyAssetsToOutput("Debug", from, OutBin .. AstFol, PrjLoc .. AstFol)
    CopyAssetsToOutput("Release", from, OutBin  .. AstFol, PrjLoc .. AstFol)
    CopyAssetsToOutput("Dist", from, OutBinDist  .. AstFol, PrjLoc .. AstFol)

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"

    filter "configurations:Dist"
        optimize "On"
