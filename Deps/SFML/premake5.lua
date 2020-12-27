project "SFML"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	
	targetdir ("Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputDirectory .. "/%{prj.name}")
	
    files
    {
		"src/**.hpp",
		"src/dummy.cpp"
    }
	
	defines
	{
		"SFML_STATIC"
	}

	libdirs 
	{ 
		"lib"
	}

	links 
	{
		"opengl32.lib",
		"freetype.lib", 
		"winmm.lib", 
		"gdi32.lib", 
		"openal32.lib",
		"flac.lib", 
		"vorbisenc.lib", 
		"vorbisfile.lib", 
		"vorbis.lib", 
		"ogg.lib", 
		"ws2_32.lib",		
	}
	
    includedirs
    {
        "include",
        "src"
    }
	
    
    filter "system:windows"
		filter "configurations:Debug"
			runtime "Debug"
			symbols "On"
			links
			{
				"sfml-graphics-s-d",
				"sfml-window-s-d",
				"sfml-audio-s-d",
				"sfml-network-s-d",
				"sfml-system-s-d",
			}

		filter "configurations:Release"
			runtime "Release"
			symbols "On"
			links
			{
				"sfml-graphics-s",
				"sfml-window-s",
				"sfml-audio-s",
				"sfml-network-s",
				"sfml-system-s",
			}

		filter "configurations:Dist"
			runtime "Release"
			optimize "On"		
			systemversion "latest"
			links
			{
				"sfml-graphics-s",
				"sfml-window-s",
				"sfml-audio-s",
				"sfml-network-s",
				"sfml-system-s",
			}
			
	