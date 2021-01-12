#
# zlib library target
#
set(VENDOR_ZLIB_DIR "${MTASA_VENDOR_DIR}/zlib")

add_library(vendor-zlib STATIC
    "${VENDOR_ZLIB_DIR}/adler32.c"
    "${VENDOR_ZLIB_DIR}/compress.c"
    "${VENDOR_ZLIB_DIR}/crc32.c"
    "${VENDOR_ZLIB_DIR}/deflate.c"
    "${VENDOR_ZLIB_DIR}/gzclose.c"
    "${VENDOR_ZLIB_DIR}/gzlib.c"
    "${VENDOR_ZLIB_DIR}/gzread.c"
    "${VENDOR_ZLIB_DIR}/gzwrite.c"
    "${VENDOR_ZLIB_DIR}/infback.c"
    "${VENDOR_ZLIB_DIR}/inffast.c"
    "${VENDOR_ZLIB_DIR}/inflate.c"
    "${VENDOR_ZLIB_DIR}/inftrees.c"
    "${VENDOR_ZLIB_DIR}/trees.c"
    "${VENDOR_ZLIB_DIR}/uncompr.c"
    "${VENDOR_ZLIB_DIR}/zutil.c"
)

target_include_directories(vendor-zlib PUBLIC "${VENDOR_ZLIB_DIR}")
