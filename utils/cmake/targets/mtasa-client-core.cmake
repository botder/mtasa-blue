#
# Client core library target
#
set(CLIENT_CORE_DIR "${MTASA_CLIENT_DIR}/core")

file(GLOB_RECURSE CLIENT_CORE_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${CLIENT_CORE_DIR}/*.cpp"
    "${CLIENT_CORE_DIR}/*.h"
    "${CLIENT_CORE_DIR}/*.hpp"
)

list(REMOVE_ITEM CLIENT_CORE_SOURCES "${CLIENT_CORE_DIR}/StdInc.cpp")

add_library(mtasa-client-core SHARED
    ${CLIENT_CORE_SOURCES}
    "${CLIENT_CORE_DIR}/core.rc"
)

target_precompile_headers(mtasa-client-core PRIVATE "${CLIENT_CORE_DIR}/StdInc.h")

set_target_properties(mtasa-client-core PROPERTIES OUTPUT_NAME "core")

mtasa_set_target_outputdir(mtasa-client-core "mta")

target_include_directories(mtasa-client-core PRIVATE
    "${CLIENT_CORE_DIR}"
)

target_compile_definitions(mtasa-client-core PRIVATE
    INITGUID
    PNG_SETJMP_NOT_SUPPORTED
)

target_link_libraries(mtasa-client-core PRIVATE
    mtasa-shared-sdk
    mtasa-client-sdk
    mtasa-client-pthread
    vendor-sparsehash
    vendor-detours
    vendor-hwbrk
    vendor-tinygettext
    vendor-d3dx9
    vendor-zlib
    vendor-jpeg
    vendor-png
    ws2_32
    xinput
    dinput8
    winmm
    imm32
    userenv

    dbghelp
    imagehlp
    dxguid
    strmiids
    odbc32
    odbccp32
    shlwapi
    gdi32
    psapi
)

target_link_options(mtasa-client-core PRIVATE
    $<$<BOOL:${MTASA_MSVC}>:
        /SAFESEH:NO
    >
)
