#
# bcrypt library target
#
set(VENDOR_BCRYPT_DIR "${MTASA_VENDOR_DIR}/bcrypt")

file(GLOB_RECURSE BCRYPT_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${VENDOR_BCRYPT_DIR}/*.c"
    "${VENDOR_BCRYPT_DIR}/*.h"
)

add_library(vendor-bcrypt STATIC ${BCRYPT_SOURCES})

target_include_directories(vendor-bcrypt PUBLIC "${VENDOR_BCRYPT_DIR}")
