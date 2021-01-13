#
# NvAPI library target
#
set(VENDOR_NVAPI_DIR "${MTASA_VENDOR_DIR}/nvapi")

add_library(vendor-nvapi INTERFACE)

if (MTASA_OS_WINDOWS)
    set_target_properties(vendor-nvapi PROPERTIES
        IMPORTED_IMPLIB

        $<IF:$<BOOL:${MTASA_X64}>
            "${VENDOR_NVAPI_DIR}/amd64/nvapi64.lib"
            ,
            "${VENDOR_NVAPI_DIR}/x86/nvapi.lib"
        >
    )
endif()

target_include_directories(vendor-nvapi INTERFACE "${VENDOR_NVAPI_DIR}")
