project "ehs"
	language "C++"
	kind "StaticLib"
	targetname "ehs"

	includedirs {
		"../../Shared/sdk",
		"../../Shared/sockets",
		"../pcre",
		"../pme",
	}

	defines { "WIN32_LEAN_AND_MEAN", "_LIB" }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "*.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"datum.cpp",
		"ehs.cpp",
		"httprequest.cpp",
		"httpresponse.cpp",
		"socket.cpp"
	}

	links { "Sockets" }

	filter "system:windows"
		includedirs { "../pthreads/include" }
		disablewarnings { "4800" }
