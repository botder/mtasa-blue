#
# Server core library target
#
set(SERVER_CORE_DIR "${MTASA_SERVER_DIR}/core")

add_library(mtasa-server-core SHARED
    "${SERVER_CORE_DIR}/CCrashHandler.cpp"
    "${SERVER_CORE_DIR}/CCrashHandler.h"
    "${SERVER_CORE_DIR}/CCrashHandlerAPI.cpp"
    "${SERVER_CORE_DIR}/CCrashHandlerAPI.h"
    "${SERVER_CORE_DIR}/CDynamicLibrary.cpp"
    "${SERVER_CORE_DIR}/CDynamicLibrary.h"
    "${SERVER_CORE_DIR}/CExceptionInformation_Impl.h"
    "${SERVER_CORE_DIR}/CModManagerImpl.cpp"
    "${SERVER_CORE_DIR}/CModManagerImpl.h"
    "${SERVER_CORE_DIR}/CServerImpl.cpp"
    "${SERVER_CORE_DIR}/CServerImpl.h"
    "${SERVER_CORE_DIR}/CThreadCommandQueue.cpp"
    "${SERVER_CORE_DIR}/CThreadCommandQueue.h"
    "${SERVER_CORE_DIR}/ErrorCodes.h"
    "${SERVER_CORE_DIR}/Server.cpp"

    $<$<BOOL:${MTASA_OS_WINDOWS}>:
        "${SERVER_CORE_DIR}/CExceptionInformation_Impl.cpp"
    >
)

target_precompile_headers(mtasa-server-core PRIVATE "${SERVER_CORE_DIR}/StdInc.h")

set_target_properties(mtasa-server-core PROPERTIES OUTPUT_NAME "core")

mtasa_set_target_outputdir(mtasa-server-core "${MTASA_SERVER_OUTPUT_DIR}")

target_link_libraries(mtasa-server-core PRIVATE
    mtasa-server-sdk
    mtasa-shared-sdk
    vendor-detours
    vendor-pthread
)

target_link_options(mtasa-server-core PRIVATE
    $<$<BOOL:${MTASA_MSVC}>:
        /SAFESEH:NO
    >
)
