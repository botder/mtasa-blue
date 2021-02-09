project "Dbconmy"
    language "C++"
    kind "SharedLib"
    targetname "dbconmy"
    targetdir(buildpath("server/mods/deathmatch"))

    includedirs {
        "../sdk",
        "../mods/deathmatch",
        "../../vendor/google-breakpad/src",
        "../../vendor/sparsehash/src",
    }

    files {
        "premake5.lua",
        "CDatabaseConnectionMySql.cpp",
        "../../Shared/sdk/SharedUtil.cpp",
    }

    filter "platforms:x64"
        targetdir(buildpath("server/x64"))
    
    filter "system:windows"
        includedirs {
            "../../vendor/mysql/include",
            "../../vendor/sparsehash/src/windows",
        }
    
    filter { "system:windows", "platforms:x86" }
        links { "../../vendor/mysql/lib/x86/libmysql.lib" }

    filter { "system:windows", "platforms:x64" }
        links { "../../vendor/mysql/lib/x64/libmysql.lib" }

    filter "system:linux"
        includedirs {
            "/usr/include/mysql",
        }

        links { "rt" }

        if GLIBC_COMPAT then
            buildoptions { "-pthread" }
            linkoptions { "-l:libmysqlclient.a", "-pthread" }
            links { "z", "dl", "m" }
        else
            links { "mysqlclient" }
        end

    filter {"system:linux", "platforms:x86"}
        libdirs { "/usr/lib32/mysql" }

    filter {"system:linux", "platforms:x64"}
        libdirs { "/usr/lib64/mysql" }

    filter "system:macosx"
        includedirs {
            os.findheader("mysql.h", "/usr/local/opt/mysql/include/mysql")
        }

        links { "mysqlclient" }
