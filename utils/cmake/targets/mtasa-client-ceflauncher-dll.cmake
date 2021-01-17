#
# Client CEFLauncher library target
#
set(CLIENT_CEFLAUNCHER_DLL_DIR "${MTASA_CLIENT_DIR}/ceflauncher_DLL")

add_library(mtasa-client-ceflauncher-dll SHARED
    "${CLIENT_CEFLAUNCHER_DLL_DIR}/Main.cpp"
    "${CLIENT_CEFLAUNCHER_DLL_DIR}/CCefApp.h"
    "${CLIENT_CEFLAUNCHER_DLL_DIR}/V8Helpers.h"
)

set_target_properties(mtasa-client-ceflauncher-dll PROPERTIES OUTPUT_NAME "CEFLauncher_DLL")

target_compile_definitions(mtasa-client-ceflauncher-dll PRIVATE
    UNICODE
)

mtasa_set_target_outputdir(mtasa-client-ceflauncher-dll "mta/cef")

target_link_libraries(mtasa-client-ceflauncher-dll PRIVATE
    vendor-cef
)
