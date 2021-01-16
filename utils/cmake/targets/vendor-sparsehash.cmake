#
# sparsehash library target
#
set(VENDOR_SPARSEHASH_DIR "${MTASA_VENDOR_DIR}/sparsehash/src")

add_library(vendor-sparsehash INTERFACE)

target_include_directories(vendor-sparsehash INTERFACE
    $<$<BOOL:${MTASA_OS_WINDOWS}>:
        "${VENDOR_SPARSEHASH_DIR}/windows"
    >

    "${VENDOR_SPARSEHASH_DIR}"
)
