#
# ksignals library target
#
if (NOT (MTASA_OS_WINDOWS AND MTASA_X86))
    return()
endif()

set(VENDOR_KSIGNALS_DIR "${MTASA_VENDOR_DIR}/ksignals")

add_library(vendor-ksignals INTERFACE)

target_include_directories(vendor-ksignals INTERFACE "${VENDOR_KSIGNALS_DIR}")
