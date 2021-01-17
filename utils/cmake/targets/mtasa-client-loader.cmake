#
# Client loader library target
#
set(CLIENT_LOADER_DIR "${MTASA_CLIENT_DIR}/loader")

add_library(mtasa-client-loader SHARED
    "${CLIENT_LOADER_DIR}/loader.rc"
    "${CLIENT_LOADER_DIR}/resource/splash.bmp"
    "${CLIENT_LOADER_DIR}/CExePatchedStatus.cpp"
    "${CLIENT_LOADER_DIR}/CExePatchedStatus.h"
    "${CLIENT_LOADER_DIR}/CFileGenerator.cpp"
    "${CLIENT_LOADER_DIR}/CFileGenerator.h"
    "${CLIENT_LOADER_DIR}/CInstallManager.cpp"
    "${CLIENT_LOADER_DIR}/CInstallManager.h"
    "${CLIENT_LOADER_DIR}/CSequencer.h"
    "${CLIENT_LOADER_DIR}/D3DStuff.cpp"
    "${CLIENT_LOADER_DIR}/D3DStuff.h"
    "${CLIENT_LOADER_DIR}/Dialogs.cpp"
    "${CLIENT_LOADER_DIR}/Dialogs.h"
    "${CLIENT_LOADER_DIR}/Install.cpp"
    "${CLIENT_LOADER_DIR}/Install.h"
    "${CLIENT_LOADER_DIR}/Main.cpp"
    "${CLIENT_LOADER_DIR}/Main.h"
    "${CLIENT_LOADER_DIR}/MainFunctions.cpp"
    "${CLIENT_LOADER_DIR}/MainFunctions.h"
    "${CLIENT_LOADER_DIR}/resource.h"
    "${CLIENT_LOADER_DIR}/StdInc.cpp" # for SharedUtil.hpp
    "${CLIENT_LOADER_DIR}/StdInc.h"
    "${CLIENT_LOADER_DIR}/Utils.cpp"
    "${CLIENT_LOADER_DIR}/Utils.h"
)

target_precompile_headers(mtasa-client-loader PRIVATE "${CLIENT_LOADER_DIR}/StdInc.h")

set_target_properties(mtasa-client-loader PROPERTIES OUTPUT_NAME "loader")

mtasa_set_target_outputdir(mtasa-client-loader "mta")

target_link_libraries(mtasa-client-loader PRIVATE
    mtasa-shared-sdk
    mtasa-client-sdk
    vendor-detours
    vendor-unrar
    vendor-nvapi
    vendor-d3dx9
    d3d9
)

target_link_options(mtasa-client-loader PRIVATE
    $<$<BOOL:${MTASA_MSVC}>:
        /SAFESEH:NO
    >
)
