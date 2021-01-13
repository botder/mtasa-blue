#
# mbedtls library target
#
if (MTASA_OS_WINDOWS)
    return()
endif()

set(VENDOR_MBEDTLS_DIR "${MTASA_VENDOR_DIR}/mbedtls")

file(GLOB_RECURSE MBEDTLS_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${VENDOR_MBEDTLS_DIR}/library/*.c"
    "${VENDOR_MBEDTLS_DIR}/include/*.h"
)

add_library(vendor-mbedtls STATIC ${MBEDTLS_SOURCES})

target_compile_definitions(vendor-mbedtls PUBLIC MBEDTLS_ZLIB_SUPPORT)

target_include_directories(vendor-mbedtls PUBLIC "${VENDOR_MBEDTLS_DIR}/include")

target_link_libraries(vendor-mbedtls PRIVATE vendor-zlib)
