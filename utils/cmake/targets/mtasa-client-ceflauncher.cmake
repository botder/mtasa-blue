#
# Client CEFLauncher executable target
#
set(CLIENT_CEFLAUNCHER_DIR "${MTASA_CLIENT_DIR}/ceflauncher")

add_executable(mtasa-client-ceflauncher WIN32
    "${CLIENT_CEFLAUNCHER_DIR}/Main.cpp"
)

set_target_properties(mtasa-client-ceflauncher PROPERTIES
    OUTPUT_NAME
    "CEFLauncher"

    DEBUG_POSTFIX
    "${CMAKE_DEBUG_POSTFIX}"
)

mtasa_set_target_outputdir(mtasa-client-ceflauncher "mta/cef")

target_link_options(mtasa-client-ceflauncher PRIVATE "/ENTRY:WinMainCRTStartup")

target_link_libraries(mtasa-client-ceflauncher PRIVATE
    mtasa-client-ceflauncher-dll
)
