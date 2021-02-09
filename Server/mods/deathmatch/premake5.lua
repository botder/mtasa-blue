project "Deathmatch"
    language "C++"
    kind "SharedLib"
    targetname "deathmatch"
    targetdir(buildpath("server/mods/deathmatch"))

    includedirs {
        ".",
        "logic",
        "utils",
        "../../sdk",
        "../../../vendor/bochs",
        "../../../vendor/pme",
        "../../../vendor/zip",
        "../../../vendor/zlib",
        "../../../vendor/pcre",
        "../../../vendor/json-c",
        "../../../vendor/bob_withers",
        "../../../vendor/lua/src",
        "../../../Shared/gta",
        "../../../Shared/mods/deathmatch/logic",
        "../../../Shared/animation",
        "../../../Shared/publicsdk/include",
        "../../../vendor/sparsehash/src/",
    }

    files {
        "premake5.lua",
        "UnityBuild.cpp",
        "utils/UnityBuild.cpp",
        "logic/UnityBuild_1.cpp",
        "logic/UnityBuild_2.cpp",
        "logic/UnityBuild_3.cpp",
        "logic/UnityBuild_4.cpp",
        "logic/UnityBuild_5.cpp",
        "logic/UnityBuild_Collision.cpp",
        "logic/UnityBuild_Console.cpp",
        "logic/UnityBuild_Database.cpp",
        "logic/UnityBuild_PerfStat.cpp",
        "logic/UnityBuild_Player.cpp",
        "logic/UnityBuild_Resource.cpp",
        "logic/UnityBuild_Vehicle.cpp",
        "logic/lua/UnityBuild.cpp",
        "logic/luadefs/UnityBuild.cpp",
        "logic/net/UnityBuild.cpp",
        "logic/packets/UnityBuild.cpp",

        -- TODO: Unity build
        "../../../Shared/mods/deathmatch/logic/**.cpp",
        "../../../Shared/animation/CEasingCurve.cpp",
        "../../../Shared/animation/CPositionRotationAnimation.cpp",

        -- TODO: Replace with cryptopp functions
        "../../../vendor/bochs/bochs_internal/bochs_crc32.cpp",
    }

    links {
        "Lua_Server",
        "sqlite",
        "ehs",
        "cryptopp",
        "pme",
        "pcre",
        "json-c",
        "zip",
        "zlib",
        "blowfish_bcrypt",
    }

    defines { "SDK_WITH_BCRYPT" }

    filter "platforms:x64"
        targetdir(buildpath("server/x64"))

    filter "system:windows"
        includedirs {
            "../../../vendor/sparsehash/src/windows",
            "../../../vendor/pthreads/include",
        }

        links {
            "ws2_32",
            "pthread",
        }

    filter "system:linux"
        links { "rt" }
        buildoptions { "-pthread" }
        linkoptions { "-pthread" }

        -- TODO: Fix source code and remove this warning suppression
        buildoptions { "-Wno-narrowing" }

    filter "system:macosx"
        buildoptions { "-pthread" }
        linkoptions { "-pthread" }

        -- TODO: Fix source code and remove this warning suppression
        buildoptions { "-Wno-narrowing" }
