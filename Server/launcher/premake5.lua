project "Launcher"
    language "C++"
    kind "ConsoleApp"
    targetdir(buildpath("server"))

    includedirs {
        "../sdk",
    }
    
    files {
        "premake5.lua",
        "UnityBuild.cpp",
    }

    filter "system:windows"
        targetname "MTA Server"
        staticruntime "On"

        files {
            "launcher.rc",
        }

    filter {"system:windows", "platforms:x64"}
        targetname "MTA Server64"

    filter "system:not windows"
        targetname "mta-server"

        links { "dl" }
        buildoptions { "-pthread", "-fvisibility=default" }
        linkoptions { "-pthread", "-rdynamic" }

    filter {"system:linux", "platforms:x64"}
        targetname "mta-server64"
