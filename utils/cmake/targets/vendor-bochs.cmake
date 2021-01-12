#
# Bochs library target
#
set(VENDOR_BOCHS_DIR "${MTASA_VENDOR_DIR}/bochs")

add_library(vendor-bochs STATIC "${VENDOR_BOCHS_DIR}/bochs_internal/crc32.cpp")

target_include_directories(vendor-bochs PUBLIC "${VENDOR_BOCHS_DIR}")

# target_link_libraries(vendor-bochs PRIVATE vendor-zlib mtasa-shared-sdk)
