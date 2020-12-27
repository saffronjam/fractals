project "ImGui"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
	
	outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
	targetdir ("Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputDirectory .. "/%{prj.name}")
	
	files
	{
        "imconfig.h",
        "imgui.h",
        "imgui.cpp",
        "imgui_draw.cpp",
        "imgui_internal.h",
        "imgui_widgets.cpp",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
        "imgui_demo.cpp"
    }
    
	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"