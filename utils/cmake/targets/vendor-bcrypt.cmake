#
# bcrypt library target
#
set(VENDOR_BCRYPT_DIR "${MTASA_VENDOR_DIR}/bcrypt")

add_library(vendor-bcrypt STATIC
    "${VENDOR_BCRYPT_DIR}/crypt_blowfish.c"
    "${VENDOR_BCRYPT_DIR}/crypt_gensalt.c"
    "${VENDOR_BCRYPT_DIR}/wrapper.c"
    "${VENDOR_BCRYPT_DIR}/crypt_blowfish.h"
    "${VENDOR_BCRYPT_DIR}/crypt_gensalt.h"
    "${VENDOR_BCRYPT_DIR}/crypt.h"
    "${VENDOR_BCRYPT_DIR}/ow-crypt.h"
    "${VENDOR_BCRYPT_DIR}/x86.S"
)

target_include_directories(vendor-bcrypt PUBLIC "${VENDOR_BCRYPT_DIR}")
