-- premake5.lua
workspace "neclib"
	configurations { "Debug", "Release"}
		toolset "v141_xp"
		nativewchar "On"
		system ("Windows")
		characterset ("MBCS")
	
project "neclib"
	kind "SharedLib"
	includedirs { "Inc", "minhook/include", "boost-1.67.0" }
	language "C++"
	targetdir "build/bin/%{cfg.buildcfg}"
	
	files { "**.h", "**.hpp", "**.c", "**.cpp", "**.def"}
	excludes { "Levitation/**", "boost-1.67.0/**" }
	
	filter "configurations:Debug"
		defines {"DEBUG", "WIN32", "_DEBUG", "_WINDOWS", "_USRDLL", "DYNITEMINST_EXPORTS"}
		symbols "On"
		
	filter "configurations:Release"
		defines {"WIN32", "NDEBUG", "_WINDOWS", "_USRDLL", "DYNITEMINST_EXPORTS"}
		optimize "On"
		
	filter "system:Windows"
      system "windows"	
		
		