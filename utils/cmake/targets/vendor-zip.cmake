#
# zip library target
#
set(VENDOR_ZIP_DIR "${MTASA_VENDOR_DIR}/zip")

add_library(vendor-zip STATIC
    "${VENDOR_ZIP_DIR}/crypt.h"
    "${VENDOR_ZIP_DIR}/ioapi.c"
    "${VENDOR_ZIP_DIR}/ioapi.h"
    "${VENDOR_ZIP_DIR}/mztools.c"
    "${VENDOR_ZIP_DIR}/mztools.h"
    "${VENDOR_ZIP_DIR}/unzip.c"
    "${VENDOR_ZIP_DIR}/unzip.h"
    "${VENDOR_ZIP_DIR}/zip.c"
    "${VENDOR_ZIP_DIR}/zip.h"

    $<$<BOOL:${MTASA_OS_WINDOWS}>:
        "${VENDOR_ZIP_DIR}/iowin32.c"
        "${VENDOR_ZIP_DIR}/iowin32.h"
    >
)

target_include_directories(vendor-zip PUBLIC "${VENDOR_ZIP_DIR}")

target_link_libraries(vendor-zip PUBLIC vendor-zlib)
