#
# Shared SDK library target
#
set(SHARED_SDK_DIR "${MTASA_SHARED_DIR}/sdk")

add_library(mtasa-shared-sdk INTERFACE)

target_include_directories(mtasa-shared-sdk INTERFACE
    "${SHARED_SDK_DIR}"
    "${MTASA_VENDOR_DIR}"
)
