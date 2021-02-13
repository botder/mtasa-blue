project "Deathmatch"
	language "C++"
	kind "SharedLib"
	targetname "deathmatch"
	targetdir(buildpath("server/mods/deathmatch"))

	includedirs {
		"../../sdk",
		"../../../vendor/bochs",
		"../../../vendor/pme",
		"../../../vendor/zip",
		"../../../vendor/zlib",
		"../../../vendor/pcre",
		"../../../vendor/json-c",
		"../../../vendor/bob_withers",
		"../../../vendor/lua/src",
		"../../../vendor/mongoose/7.1",
		"../../../vendor/mbedtls/include", -- for: mongoose
		"../../../Shared/gta",
		"../../../Shared/mods/deathmatch/logic",
		"../../../Shared/animation",
		"../../../Shared/publicsdk/include",
		"../../../vendor/sparsehash/src",
		"logic",
		"utils",
		".",
	}

	vpaths {
		["Headers/*"] = {"**.h", "../../../Shared/mods/deathmatch/**.h", "../../**.h"},
		["Sources/*"] = {"**.cpp", "../../../Shared/mods/deathmatch/**.cpp", "../../../Shared/**.cpp", "../../../vendor/**.cpp", "../../**.cpp"},
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.h",
		"**.cpp",
		"../../../Shared/mods/deathmatch/logic/**.cpp",
		"../../../Shared/mods/deathmatch/logic/**.h",
		"../../../Shared/animation/CEasingCurve.cpp",
		"../../../Shared/animation/CPositionRotationAnimation.cpp",
		-- Todo: Replace these two by using the CryptoPP functions instead
		"../../../vendor/bochs/bochs_internal/bochs_crc32.cpp",
	}

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"
	
	defines {
		"SDK_WITH_BCRYPT",
		"MG_ENABLE_MBEDTLS", -- for: mongoose
	}

	links {
		"Lua_Server",
		"sqlite",
		"cryptopp",
		"pme",
		"pcre",
		"json-c",
		"zip",
		"zlib",
		"blowfish_bcrypt",
		"mongoose",
	}

	filter "system:windows"
		includedirs {
			"../../../vendor/pthreads/include",
			"../../../vendor/sparsehash/src/windows",
		}
		buildoptions { "-Zm130" }
		links { "ws2_32", "pthread" }

	filter "system:not windows"
		buildoptions { "-Wno-narrowing" } -- We should fix the warnings at some point
		buildoptions { "-pthread" }
		linkoptions { "-pthread" }

	filter "system:linux"
		links { "rt" }

	filter "platforms:x64"
		targetdir(buildpath("server/x64"))
