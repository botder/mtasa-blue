#
# Client SDK library target
#
set(CLIENT_SDK_DIR "${MTASA_CLIENT_DIR}/sdk")

add_library(mtasa-client-sdk INTERFACE)

target_include_directories(mtasa-client-sdk INTERFACE ${CLIENT_SDK_DIR})
