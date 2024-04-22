local DLL_32_BIT_PATH = "../vendor/wasm/x86/libiwasm.dll";
local DLL_64_BIT_PATH = "../vendor/wasm/x64/libiwasm.dll";

project "Wasm_Server"
	language "C++"
	targetname "wasm_runtime"

	vpaths {
        ["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua"
	}

	filter "system:windows"
		kind "SharedLib"
		targetdir(buildpath("server/mods/deathmatch"))
		postbuildcommands {
			"{COPYFILE} " .. DLL_32_BIT_PATH .. " %{cfg.targetdir}"
		}

	filter "system:not windows"
		kind "StaticLib"

	filter {"system:windows", "platforms:x64"}
		targetdir(buildpath("server/x64"))
		postbuildcommands {
			"{COPYFILE} " .. DLL_64_BIT_PATH .. " %{cfg.targetdir}"
		}

	filter {"system:windows", "platforms:arm"}
		targetdir(buildpath("server/arm"))
		postbuildcommands {
			"{COPYFILE} " .. DLL_64_BIT_PATH .. " %{cfg.targetdir}"
		}

	filter {"system:windows", "platforms:arm64"}
		targetdir(buildpath("server/arm64"))
		postbuildcommands {
			"{COPYFILE} " .. DLL_64_BIT_PATH .. " %{cfg.targetdir}"
		}

if os.target() == "windows" then
	project "Wasm_Client"
		language "C++"
		kind "SharedLib"
		targetname "wasm_runtimec"
		targetdir(buildpath("mods/deathmatch"))

		vpaths {
            ["*"] = "premake5.lua"
		}
		
		postbuildcommands {
			"{COPYFILE} " .. DLL_32_BIT_PATH .. " %{cfg.targetdir}"
		}

        filter "platforms:not x86"
            flags { "ExcludeFromBuild" } 
end
