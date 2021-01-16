#
# NvAPI library target
#
set(VENDOR_NVAPI_DIR "${MTASA_VENDOR_DIR}/nvapi")

add_library(vendor-nvapi STATIC IMPORTED)

if (MTASA_OS_WINDOWS)
    if (MTASA_X64)
        set_target_properties(vendor-nvapi PROPERTIES
            IMPORTED_IMPLIB
            "${VENDOR_NVAPI_DIR}/amd64/nvapi64.lib"
        )
    else()
        set_target_properties(vendor-nvapi PROPERTIES
            IMPORTED_IMPLIB
            "${VENDOR_NVAPI_DIR}/x86/nvapi.lib"
        )
    endif()
endif()

target_include_directories(vendor-nvapi INTERFACE "${VENDOR_NVAPI_DIR}")
