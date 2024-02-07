project "Launcher"
	language "C++"
	kind "ConsoleApp"
	targetdir(buildpath("server"))
	targetname "GninE-server"

	includedirs {
		"../../Shared/sdk",
		"../sdk",
	}

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Resources/*"] = {"*.rc", "**.ico"},
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}

	filter "system:windows"
		targetname "GninE Server"
		staticruntime "On"
		files {
			"launcher.rc",
			"resource/GninEicon.ico"
		}

	filter "system:not windows"
		links { "dl" }
		buildoptions { "-pthread", "-fvisibility=default" }
		linkoptions { "-pthread", "-rdynamic" }

	filter "platforms:arm"
		targetname "GninE-server-arm"

	filter "platforms:arm64"
		targetname "GninE-server-arm64"

	filter { "system:linux", "platforms:x64" }
		targetname "GninE-server64"

	filter { "system:windows", "platforms:x64" }
		targetname "GninE Server64"

	filter { "system:windows", "platforms:arm" }
		targetname "GninE Server ARM"

	filter { "system:windows", "platforms:arm64" }
		targetname "GninE Server ARM64"
