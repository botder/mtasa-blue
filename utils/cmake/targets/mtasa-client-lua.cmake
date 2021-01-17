#
# Client lua library target
#
add_library(mtasa-client-lua SHARED $<TARGET_PROPERTY:vendor-lua,SOURCES>)

target_include_directories(mtasa-client-lua PUBLIC
    $<TARGET_PROPERTY:vendor-lua,INCLUDE_DIRECTORIES>
)

target_include_directories(mtasa-client-lua PUBLIC "${VENDOR_LUA_DIR}")

target_compile_definitions(mtasa-client-lua PUBLIC
    LUA_BUILD_AS_DLL
    LUA_USE_APICHECK
)

set_target_properties(mtasa-client-lua PROPERTIES OUTPUT_NAME "lua5.1c")

mtasa_set_target_outputdir(mtasa-client-lua "mods/deathmatch")
