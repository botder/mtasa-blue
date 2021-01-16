#
# Server deathmatch library target
#
set(SERVER_DEATHMATCH_DIR "${MTASA_SERVER_DIR}/mods/deathmatch")
set(SHARED_DEATHMATCH_DIR "${MTASA_SHARED_DIR}/mods/deathmatch")
set(SHARED_ANIMATION_DIR "${MTASA_SHARED_DIR}/animation")
set(SHARED_GTA_DIR "${MTASA_SHARED_DIR}/gta")
set(VENDOR_BOCHS_DIR "${MTASA_VENDOR_DIR}/bochs")

file(GLOB_RECURSE SERVER_DEATHMATCH_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${SERVER_DEATHMATCH_DIR}/logic/*.cpp"
    "${SERVER_DEATHMATCH_DIR}/logic/*.h"
)

file(GLOB_RECURSE SHARED_DEATHMATCH_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${SHARED_DEATHMATCH_DIR}/logic/*.cpp"
    "${SHARED_DEATHMATCH_DIR}/logic/*.h"
)

add_library(mtasa-server-deathmatch SHARED
    "${SERVER_DEATHMATCH_DIR}/CServer.cpp"
    "${SERVER_DEATHMATCH_DIR}/CServer.h"
    "${SERVER_DEATHMATCH_DIR}/Server.cpp"
    "${SERVER_DEATHMATCH_DIR}/Config.h"
    "${SERVER_DEATHMATCH_DIR}/utils/CFunctionUseLogger.cpp"
    "${SERVER_DEATHMATCH_DIR}/utils/CFunctionUseLogger.h"
    "${SERVER_DEATHMATCH_DIR}/utils/CHqComms.h"
    "${SERVER_DEATHMATCH_DIR}/utils/CMasterServerAnnouncer.h"
    "${SERVER_DEATHMATCH_DIR}/utils/COpenPortsTester.h"
    "${SERVER_DEATHMATCH_DIR}/utils/CZipMaker.cpp"
    "${SERVER_DEATHMATCH_DIR}/utils/CZipMaker.h"
    ${SERVER_DEATHMATCH_SOURCES}

    "${SHARED_ANIMATION_DIR}/CEasingCurve.cpp"
    "${SHARED_ANIMATION_DIR}/CPositionRotationAnimation.cpp"
    "${SHARED_ANIMATION_DIR}/CEasingCurve.h"
    "${SHARED_ANIMATION_DIR}/CPositionRotationAnimation.h"
    "${SHARED_ANIMATION_DIR}/TInterpolation.h"
    "${SHARED_ANIMATION_DIR}/EasingDeclarations.hpp"
    "${SHARED_ANIMATION_DIR}/EasingEquations.hpp"
    ${SHARED_DEATHMATCH_SOURCES}

    "${VENDOR_BOCHS_DIR}/bochs_internal/crc32.cpp"
)

target_include_directories(mtasa-server-deathmatch PRIVATE
    "${VENDOR_BOCHS_DIR}"
    "${SHARED_GTA_DIR}"
    "${SHARED_ANIMATION_DIR}"
    "${SHARED_DEATHMATCH_DIR}/logic"
    "${SERVER_DEATHMATCH_DIR}"
    "${SERVER_DEATHMATCH_DIR}/logic"
)

target_link_libraries(mtasa-server-deathmatch PRIVATE
    vendor-cryptopp
    vendor-sparsehash
    vendor-pme
    vendor-json
    vendor-zlib
    vendor-zip
    vendor-pcre
    vendor-sqlite
    vendor-ehs
    vendor-bcrypt
    mtasa-shared-sdk
    mtasa-server-sdk
    mtasa-server-pthread
    mtasa-server-pcre
    mtasa-server-lua
    mtasa-public-sdk
)

target_compile_definitions(mtasa-server-deathmatch PRIVATE SDK_WITH_BCRYPT)

target_precompile_headers(mtasa-server-deathmatch PRIVATE "${SERVER_DEATHMATCH_DIR}/StdInc.h")

set_target_properties(mtasa-server-deathmatch PROPERTIES OUTPUT_NAME "deathmatch")

mtasa_set_target_outputdir(mtasa-server-deathmatch "${MTASA_SERVER_MOD_OUTPUT_DIR}")
