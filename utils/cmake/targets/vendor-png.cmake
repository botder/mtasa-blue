#
# png library target
#
if (NOT (MTASA_OS_WINDOWS AND MTASA_X86))
    return()
endif()

set(VENDOR_PNG_DIR "${MTASA_VENDOR_DIR}/libpng")

file(GLOB_RECURSE PNG_HEADERS LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${VENDOR_PNG_DIR}/*.c"
    "${VENDOR_PNG_DIR}/*.h"
)

add_library(vendor-png STATIC ${PNG_HEADERS})

target_include_directories(vendor-png PUBLIC "${VENDOR_PME_DIR}")

target_compile_definitions(vendor-png PRIVATE
    NDEBUG
    WIN32_LEAN_AND_MEAN
    PNG_SETJMP_NOT_SUPPORTED
)

target_link_libraries(vendor-png PRIVATE vendor-zlib)
