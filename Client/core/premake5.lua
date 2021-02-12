project "Client Core"
	language "C++"
	kind "SharedLib"
	targetname "core"
	targetdir(buildpath("mta"))

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/src/windows" }
		linkoptions { "/SAFESEH:NO" }
		buildoptions { "-Zm130" }

	filter {}
		includedirs {
			".",
			"../sdk",
			"../../vendor/tinygettext",
			"../../vendor/zlib",
			"../../vendor/jpeg-9d",
			"../../vendor/pthreads/include",
			"../../vendor/sparsehash/src/",
			"../../vendor/detours/4.0.1/src",
		}

	forceincludes {
		"SharedUtil.h",
	}

	vpaths {
		["Headers/*"] = {"**.h", "**.hpp"},
		["Sources/*"] = "**.cpp",
		["Resources/*"] = {"**.rc", "../launch/resource/mtaicon.ico"},
		["*"] = "premake5.lua"
	}

	flags { "MultiProcessorCompile" }

	links { "detours" }

	files {
		"premake5.lua",
		"../launch/resource/mtaicon.ico",
		"core.rc",
		"**.h",
		"**.hpp",
		"**.cpp"
	}

	links {
		"ws2_32", "d3dx9", "Userenv", "DbgHelp", "xinput", "Imagehlp", "dxguid", "dinput8",
		"strmiids",	"odbc32", "odbccp32", "shlwapi", "winmm", "gdi32", "Imm32", "Psapi",
		"pthread", "libpng", "jpeg", "zlib", "tinygettext",
	}

	defines {
		"INITGUID",
		"PNG_SETJMP_NOT_SUPPORTED",
		"MTA_CLIENT",
		"SHARED_UTIL_WITH_FAST_HASH_MAP",
		"SHARED_UTIL_WITH_SYS_INFO",
	}

	disablewarnings {
		"4244", -- 'conversion' conversion from 'type1' to 'type2', possible loss of data
		"4995", -- 'function': name was marked as #pragma deprecated
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
