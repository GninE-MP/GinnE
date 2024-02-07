project "Client Launcher"
	language "C++"
	kind "WindowedApp"
	targetname "GninE"
	targetdir(buildpath("."))
	debugdir(buildpath("."))

	includedirs {
		"../../Shared/sdk",
		"../sdk",
	}

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	entrypoint "WinMainCRTStartup"

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Resources/*"] = {"*.rc", "**.ico", "**.xml"},
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}

	filter "system:windows"
		staticruntime "On"
		files {
			"NEU/gameicon_NEU.ico",
			"NEU/GninE.gdf.xml",
			"launch.rc",
			"GninE.rc",
			"resource/GninEicon.ico"
		}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
