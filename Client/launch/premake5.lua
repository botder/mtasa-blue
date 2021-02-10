project "Client Launcher"
    language "C++"
    kind "WindowedApp"
    targetname "Multi Theft Auto"
    targetdir(buildpath("."))
    debugdir(buildpath("."))
    entrypoint "WinMainCRTStartup"
    staticruntime "On"

    includedirs {
        "../sdk",
    }

    files {
        "premake5.lua",
        "Launcher.cpp",
        "NEU/GDFImp.gdf.xml",
        "GDFImp.rc",
        "launch.rc",
        "mtaicon.ico",
    }

    filter "architecture:x64"
        flags { "ExcludeFromBuild" }

    filter "system:not windows"
        flags { "ExcludeFromBuild" }
