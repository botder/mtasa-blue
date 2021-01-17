#
# Client game library target
#
set(CLIENT_GAME_DIR "${MTASA_CLIENT_DIR}/game_sa")

file(GLOB_RECURSE CLIENT_GAME_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${CLIENT_GAME_DIR}/*.cpp"
    "${CLIENT_GAME_DIR}/*.h"
)

add_library(mtasa-client-game SHARED ${CLIENT_GAME_SOURCES})

target_precompile_headers(mtasa-client-game PRIVATE "${CLIENT_GAME_DIR}/StdInc.h")

mtasa_set_target_outputdir(mtasa-client-game "mta")

set_target_properties(mtasa-client-game PROPERTIES
    OUTPUT_NAME "game_sa"
    CXX_STANDARD 14
)

target_link_libraries(mtasa-client-game PRIVATE
    mtasa-shared-sdk
    mtasa-client-sdk
    vendor-sparsehash
)
