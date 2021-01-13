#
# Detours library target
#
if (NOT MTASA_OS_WINDOWS)
    return()
endif()

set(VENDOR_DETOURS_DIR "${MTASA_VENDOR_DIR}/detours")

add_library(vendor-detours STATIC IMPORTED)

set_target_properties(vendor-detours PROPERTIES
    IMPORTED_LOCATION
    "${VENDOR_DETOURS_DIR}/lib/detours.lib"
)

target_include_directories(vendor-detours INTERFACE
    "${VENDOR_DETOURS_DIR}/include"
)
