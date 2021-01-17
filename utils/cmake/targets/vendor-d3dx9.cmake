#
# d3dx9 library target
#
if (NOT (MTASA_OS_WINDOWS AND MTASA_X86))
    return()
endif()

if (NOT DEFINED ENV{DXSDK_DIR})
    message(FATAL_ERROR "Environment variable DXSDK_DIR is not set!")
endif()

set(VENDOR_DXSDK_DIR "$ENV{DXSDK_DIR}")

if (NOT EXISTS "${VENDOR_DXSDK_DIR}/Include/d3d9.h")
    message(FATAL_ERROR "Header file d3d9.h not found!")
endif()

add_library(vendor-d3dx9 SHARED IMPORTED)

target_include_directories(vendor-d3dx9 INTERFACE
    "${VENDOR_DXSDK_DIR}/Include"
)

if (MTASA_X64)
    target_link_directories(vendor-d3dx9 INTERFACE
        "${VENDOR_DXSDK_DIR}/Lib/x64"
    )

    set_target_properties(vendor-d3dx9 PROPERTIES
        IMPORTED_IMPLIB
        "${VENDOR_DXSDK_DIR}/Lib/x64/d3dx9.lib"
    )
else()
    target_link_directories(vendor-d3dx9 INTERFACE
        "${VENDOR_DXSDK_DIR}/Lib/x86"
    )
    
    set_target_properties(vendor-d3dx9 PROPERTIES
        IMPORTED_IMPLIB
        "${VENDOR_DXSDK_DIR}/Lib/x86/d3dx9.lib"
    )
endif()
