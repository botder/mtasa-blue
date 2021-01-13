#
# CEF library target
#
if (NOT (MTASA_OS_WINDOWS AND MTASA_X86))
    return()
endif()

set(VENDOR_CEF_DIR "${MTASA_VENDOR_DIR}/cef3")

file(GLOB_RECURSE CEF_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${VENDOR_CEF_DIR}/*.cc"
    "${VENDOR_CEF_DIR}/*.h"
)

add_library(vendor-cef STATIC ${CEF_SOURCES})

target_include_directories(vendor-cef PUBLIC "${VENDOR_CEF_DIR}")

target_compile_definitions(vendor-cef PUBLIC
    WRAPPING_CEF_SHARED
    __STDC_CONSTANT_MACROS
    __STDC_FORMAT_MACROS
    _FILE_OFFSET_BITS=64
    UNICODE
    _UNICODE
    _WINDOWS
    _WIN32_WINNT=0x602
    WINVER=0x0602
    NOMINMAX
    WIN32_LEAN_AND_MEAN
    _HAS_EXCEPTIONS=0
    PSAPI_VERSION=1
)
