#
# hwbrk library target
#
if (NOT (MTASA_OS_WINDOWS AND MTASA_X86))
    return()
endif()

set(VENDOR_HWBRK_DIR "${MTASA_VENDOR_DIR}/hwbrk")

add_library(vendor-hwbrk STATIC
    "${VENDOR_HWBRK_DIR}/hwbrk.cpp"
    "${VENDOR_HWBRK_DIR}/hwbrk.h"
)

target_include_directories(vendor-hwbrk PUBLIC "${VENDOR_HWBRK_DIR}")
