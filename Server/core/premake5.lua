project "Core"
    language "C++"
    kind "SharedLib"
    targetname "core"
    targetdir(buildpath("server"))

    files {
        "premake5.lua",
        "UnityBuild.cpp",
    }

    includedirs {
        "../sdk",
        "../../vendor/google-breakpad/src",
        "../../vendor/sparsehash/current/src/",
    }

    filter "platforms:x64"
        targetdir(buildpath("server/x64"))

    filter "system:windows"
        includedirs { "../../vendor/sparsehash/current/src/windows" }
        linkoptions { "/SAFESEH:NO" }

    filter { "system:windows", "platforms:x86" }
        includedirs { "../../vendor/detours/4.0.1/src" }

        links {
            "detours",
            "imagehlp",
        }

    filter "system:linux"
        links {
            "ncursesw",
            "breakpad",
            "rt",
        }

        buildoptions { "-pthread" }
        linkoptions { "-pthread" }

    filter "system:macosx"
        defines { "_XOPEN_SOURCE_EXTENDED=1" }

        links {
            "CoreFoundation.framework",
            "breakpad",
            "ncurses",
        }

        buildoptions { "-pthread" }
        linkoptions { "-pthread" }
