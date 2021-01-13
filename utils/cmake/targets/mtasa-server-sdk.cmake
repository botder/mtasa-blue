#
# Server SDK library target
#
set(SERVER_SDK_DIR "${MTASA_SERVER_DIR}/sdk")

add_library(mtasa-server-sdk INTERFACE)

target_include_directories(mtasa-server-sdk INTERFACE ${SERVER_SDK_DIR})
