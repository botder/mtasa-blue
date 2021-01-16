#
# Public SDK library target
#
set(SHARED_PUBLIC_SDK_DIR "${MTASA_SHARED_DIR}/publicsdk")

add_library(mtasa-public-sdk INTERFACE)

target_include_directories(mtasa-public-sdk INTERFACE "${SHARED_PUBLIC_SDK_DIR}/include")
