#
# Client deathmatch library target
#
set(CLIENT_DEATHMATCH_DIR "${MTASA_CLIENT_DIR}/mods/deathmatch")
set(SHARED_DEATHMATCH_DIR "${MTASA_SHARED_DIR}/mods/deathmatch")
set(SHARED_ANIMATION_DIR "${MTASA_SHARED_DIR}/animation")
set(VENDOR_BOCHS_DIR "${MTASA_VENDOR_DIR}/bochs")
# set(SHARED_GTA_DIR "${MTASA_SHARED_DIR}/gta")

file(GLOB_RECURSE CLIENT_DEATHMATCH_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${CLIENT_DEATHMATCH_DIR}/logic/*.cpp"
    "${CLIENT_DEATHMATCH_DIR}/logic/*.h"
)

file(GLOB_RECURSE SHARED_DEATHMATCH_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${SHARED_DEATHMATCH_DIR}/logic/*.cpp"
    "${SHARED_DEATHMATCH_DIR}/logic/*.h"
)

add_library(mtasa-client-deathmatch SHARED
    "${CLIENT_DEATHMATCH_DIR}/CClient.cpp"
    "${CLIENT_DEATHMATCH_DIR}/CClient.h"
    "${CLIENT_DEATHMATCH_DIR}/Client.cpp"
    "${CLIENT_DEATHMATCH_DIR}/ClientCommands.cpp"
    "${CLIENT_DEATHMATCH_DIR}/ClientCommands.h"
    "${CLIENT_DEATHMATCH_DIR}/CVoiceRecorder.cpp"
    "${CLIENT_DEATHMATCH_DIR}/CVoiceRecorder.h"
    "${CLIENT_DEATHMATCH_DIR}/HeapTrace.cpp"
    "${CLIENT_DEATHMATCH_DIR}/HeapTrace.h"
    "${CLIENT_DEATHMATCH_DIR}/StdInc.h"
    ${CLIENT_DEATHMATCH_SOURCES}

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

target_include_directories(mtasa-client-deathmatch PRIVATE
    "${VENDOR_BOCHS_DIR}"
    "${SHARED_GTA_DIR}"
    "${SHARED_ANIMATION_DIR}"
    "${SHARED_DEATHMATCH_DIR}/logic"
    "${CLIENT_DEATHMATCH_DIR}"
    "${CLIENT_DEATHMATCH_DIR}/logic"
)

target_link_libraries(mtasa-client-deathmatch PRIVATE
    mtasa-shared-sdk
    mtasa-client-sdk
    mtasa-client-lua
    mtasa-client-pcre
    mtasa-client-pthread
    vendor-sparsehash
    vendor-cryptopp
    vendor-speex
    vendor-speex-dsp
    vendor-bcrypt
    vendor-zlib
    vendor-portaudio
    vendor-json
    vendor-bass
    vendor-bass-sdk
    vendor-bass-mix
    vendor-bass-fx
    vendor-bass-tags
    ws2_32
)

target_compile_definitions(mtasa-client-deathmatch PRIVATE SDK_WITH_BCRYPT)

target_precompile_headers(mtasa-client-deathmatch PRIVATE "${CLIENT_DEATHMATCH_DIR}/StdInc.h")

set_target_properties(mtasa-client-deathmatch PROPERTIES OUTPUT_NAME "client")

mtasa_set_target_outputdir(mtasa-client-deathmatch "mods/deathmatch")

target_link_options(mtasa-client-deathmatch PRIVATE
    $<$<BOOL:${MTASA_MSVC}>:
        /SAFESEH:NO
    >
)
