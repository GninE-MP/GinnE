project "Client Core"
	language "C++"
	kind "SharedLib"
	targetname "core"
	targetdir(buildpath("GninE"))

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/src/windows" }
		linkoptions { "/SAFESEH:NO" }
		buildoptions { "-Zm130" }

	filter {}
		includedirs {
			"../../Shared/sdk",
			".",
			"../sdk",
			"../../vendor/tinygettext",
			"../../vendor/zlib",
			"../../vendor/jpeg-9e",
			"../../vendor/pthreads/include",
			"../../vendor/sparsehash/src/",
			"../../vendor/detours/4.0.1/src",
			"../../vendor/discord-rpc/discord/include",
		}

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	vpaths {
		["Headers/*"] = {"**.h", "**.hpp"},
		["Sources/*"] = "**.cpp",
		["Resources/*"] = {"**.rc", "../launch/resource/GninEicon.ico"},
		["*"] = "premake5.lua"
	}

	links { "detours" }

	files {
		"premake5.lua",
		"../launch/resource/GninEicon.ico",
		"core.rc",
		"**.h",
		"**.hpp",
		"**.cpp"
	}

	links {
		"ws2_32", "d3dx9", "Userenv", "DbgHelp", "xinput", "Imagehlp", "dxguid", "dinput8",
		"strmiids",	"odbc32", "odbccp32", "shlwapi", "winmm", "gdi32", "Imm32", "Psapi",
		"pthread", "libpng", "jpeg", "zlib", "tinygettext", "discord-rpc",
	}

	defines {
		"INITGUID",
		"PNG_SETJMP_NOT_SUPPORTED"
	}

	filter "architecture:not x86"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
