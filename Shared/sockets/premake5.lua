project "Sockets"
	language "C++"
	kind "StaticLib"

	defines {
		"WIN32_LEAN_AND_MEAN",
	}

	vpaths {
		["Headers/*"] = { "mtasa/**.h" },
		["Sources/*"] = { "mtasa/**.cpp" },
		["*"] = "premake5.lua",
	}

	files {
		"premake5.lua",
		"mtasa/Endianness.h",
		"mtasa/IPAddress.cpp",
		"mtasa/IPAddress.h",
		"mtasa/IPAddressFamily.cpp",
		"mtasa/IPAddressFamily.h",
		"mtasa/IPAddressBinding.h",
		"mtasa/IPAddressMode.h",
		"mtasa/IPEndpoint.cpp",
		"mtasa/IPEndpoint.h",
		"mtasa/IPSocket.cpp",
		"mtasa/IPSocket.h",
		"mtasa/IPSocketProtocol.cpp",
		"mtasa/IPSocketProtocol.h",
		"mtasa/SocketAddress.h",
		"mtasa/SocketHandle.h",
	}

	filter "system:windows"
		links { "ws2_32" }
