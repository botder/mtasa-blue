#
# JSON library target
#
set(VENDOR_JSON_DIR "${MTASA_VENDOR_DIR}/json-c")

add_library(vendor-json STATIC
    "${VENDOR_JSON_DIR}/arraylist.c"
    "${VENDOR_JSON_DIR}/debug.c"
    "${VENDOR_JSON_DIR}/json_c_version.c"
    "${VENDOR_JSON_DIR}/json_object_iterator.c"
    "${VENDOR_JSON_DIR}/json_object.c"
    "${VENDOR_JSON_DIR}/json_pointer.c"
    "${VENDOR_JSON_DIR}/json_tokener.c"
    "${VENDOR_JSON_DIR}/json_util.c"
    "${VENDOR_JSON_DIR}/libjson.c"
    "${VENDOR_JSON_DIR}/linkhash.c"
    "${VENDOR_JSON_DIR}/printbuf.c"
    "${VENDOR_JSON_DIR}/random_seed.c"
    "${VENDOR_JSON_DIR}/strerror_override.c"
)

target_include_directories(vendor-json PUBLIC "${VENDOR_JSON_DIR}")

target_compile_definitions(vendor-json PUBLIC
    _LIB
    
    $<$<BOOL:${MTASA_PLATFORM_APPLE}>:
        HAVE_XLOCALE_H
    >
)
