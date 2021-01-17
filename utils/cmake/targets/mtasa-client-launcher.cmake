#
# Client launcher executable target
#
set(CLIENT_LAUNCHER_DIR "${MTASA_CLIENT_DIR}/launch")

add_executable(mtasa-client-launcher WIN32
    "${CLIENT_LAUNCHER_DIR}/Main.cpp"
    "${CLIENT_LAUNCHER_DIR}/GDFImp.rc"
    "${CLIENT_LAUNCHER_DIR}/launch.rc"
    "${CLIENT_LAUNCHER_DIR}/resource.h"
    "${CLIENT_LAUNCHER_DIR}/StdInc.h"
    "${CLIENT_LAUNCHER_DIR}/StdInc.cpp" # for SharedUtil.hpp
)

set_target_properties(mtasa-client-launcher PROPERTIES
    OUTPUT_NAME
    "Multi Theft Auto"

    DEBUG_POSTFIX
    "${CMAKE_DEBUG_POSTFIX}"
)

mtasa_set_target_outputdir(mtasa-client-launcher "")

target_precompile_headers(mtasa-client-launcher PRIVATE "${CLIENT_LAUNCHER_DIR}/StdInc.h")

target_link_options(mtasa-client-launcher PRIVATE "/ENTRY:WinMainCRTStartup")

target_link_libraries(mtasa-client-launcher PRIVATE
    mtasa-shared-sdk
    mtasa-client-sdk
)
