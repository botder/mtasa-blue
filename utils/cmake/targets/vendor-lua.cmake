#
# Lua library target
#
set(VENDOR_LUA_DIR "${MTASA_VENDOR_DIR}/lua/src")

file(GLOB_RECURSE LUA_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${VENDOR_LUA_DIR}/*.c"
    "${VENDOR_LUA_DIR}/*.h"
)

add_library(vendor-lua OBJECT ${LUA_SOURCES})

target_include_directories(vendor-lua PUBLIC "${VENDOR_LUA_DIR}")
