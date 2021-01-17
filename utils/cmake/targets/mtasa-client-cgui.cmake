#
# Client CGUI library target
#
set(CLIENT_CGUI_DIR "${MTASA_CLIENT_DIR}/gui")

file(GLOB_RECURSE CLIENT_CGUI_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${CLIENT_CGUI_DIR}/*.cpp"
    "${CLIENT_CGUI_DIR}/*.h"
)

list(REMOVE_ITEM CLIENT_CGUI_SOURCES "${CLIENT_CGUI_DIR}/StdInc.cpp")

add_library(mtasa-client-cgui SHARED ${CLIENT_CGUI_SOURCES})

target_precompile_headers(mtasa-client-cgui PRIVATE "${CLIENT_CGUI_DIR}/StdInc.h")

set_target_properties(mtasa-client-cgui PROPERTIES OUTPUT_NAME "cgui")

mtasa_set_target_outputdir(mtasa-client-cgui "mta")

target_link_libraries(mtasa-client-cgui PRIVATE
    mtasa-shared-sdk
    mtasa-client-sdk
    vendor-sparsehash
    vendor-cegui
    vendor-cegui-d3dx9
    vendor-cegui-falagard
    vendor-d3dx9
    dxerr
)
