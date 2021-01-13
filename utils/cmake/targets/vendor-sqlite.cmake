#
# SQLite library target
#
set(VENDOR_SQLITE_DIR "${MTASA_VENDOR_DIR}/sqlite")

add_library(vendor-sqlite STATIC
    "${VENDOR_SQLITE_DIR}/sqlite3.c"
    "${VENDOR_SQLITE_DIR}/sqlite3.h"
    "${VENDOR_SQLITE_DIR}/sqlite3ext.h"
)

target_include_directories(vendor-sqlite PUBLIC "${VENDOR_SQLITE_DIR}")
