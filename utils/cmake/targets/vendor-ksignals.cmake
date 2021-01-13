#
# ksignals library target
#
set(VENDOR_KSIGNALS_DIR "${MTASA_VENDOR_DIR}/ksignals")

add_library(vendor-ksignals INTERFACE)

target_include_directories(vendor-ksignals INTERFACE "${VENDOR_KSIGNALS_DIR}")
