project "mongoose"
    language "C"
    kind "StaticLib"

    includedirs {
        "../mbedtls/include",
    }

    vpaths {
        ["Headers/*"] = "7.1/**.h",
        ["Sources/*"] = "7.1/**.c",
        ["*"] = "premake5.lua"
    }

    files {
        "premake5.lua",
        "7.1/mongoose.c",
        "7.1/mongoose.h",
    }

    defines {
        "MG_ENABLE_MBEDTLS",
    }

    disablewarnings {
        "4005", -- because of _CRT_SECURE_NO_WARNINGS
        "4244", -- casting of 'time_t' to 'unsigned long'
    }

    links {
        "mbedtls",
    }
