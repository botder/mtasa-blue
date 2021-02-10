project "Client Deathmatch"
    language "C++"
    kind "SharedLib"
    targetname "client"
    targetdir(buildpath("mods/deathmatch"))

    includedirs {
        ".",
        "logic",
        "../../sdk/",
        "../../../vendor/pthreads/include",
        "../../../vendor/bochs",
        "../../../vendor/bass",
        "../../../vendor/libspeex",
        "../../../vendor/zlib",
        "../../../vendor/pcre",
        "../../../vendor/json-c",
        "../../../vendor/bob_withers",
        "../../../vendor/lua/src",
        "../../../Shared/mods/deathmatch/logic",
        "../../../Shared/animation",
        "../../../vendor/sparsehash/src",
        "../../../vendor/sparsehash/src/windows",
    }

    files {
        "premake5.lua",
        "UnityBuild.cpp",
        "logic/UnityBuild_1.cpp",
        "logic/UnityBuild_2.cpp",
        "logic/UnityBuild_3.cpp",
        "logic/UnityBuild_4.cpp",
        "logic/UnityBuild_5.cpp",
        "logic/UnityBuild_6.cpp",
        "logic/UnityBuild_7.cpp",
        "logic/UnityBuild_8.cpp",
        "logic/lua/UnityBuild.cpp",
        "logic/luadefs/UnityBuild_1.cpp",
        "logic/luadefs/UnityBuild_2.cpp",
        "logic/rpc/UnityBuild.cpp",
        "../../../Shared/UnityBuild_1.cpp",
        "../../../Shared/UnityBuild_2.cpp",

        -- TODO: Replace with cryptopp functions
        "../../../vendor/bochs/bochs_internal/bochs_crc32.cpp",
    }

    linkoptions { "/SAFESEH:NO" }

    links {
        "Lua_Client",
        "pcre",
        "json-c",
        "ws2_32",
        "portaudio",
        "zlib",
        "cryptopp",
        "libspeex",
        "blowfish_bcrypt",
        "../../../vendor/bass/lib/bass",
        "../../../vendor/bass/lib/bass_fx",
        "../../../vendor/bass/lib/bassmix",
        "../../../vendor/bass/lib/tags",
    }

    defines {
        "LUA_USE_APICHECK",
        "SDK_WITH_BCRYPT",
    }

    filter "architecture:x64"
        flags { "ExcludeFromBuild" }

    filter "system:not windows"
        flags { "ExcludeFromBuild" }
