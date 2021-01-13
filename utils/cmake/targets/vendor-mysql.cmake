#
# MySQL library target
#
set(VENDOR_MYSQL_DIR "${MTASA_VENDOR_DIR}/mysql")

add_library(vendor-mysql INTERFACE)

if (MTASA_OS_WINDOWS)
    set_target_properties(vendor-mysql PROPERTIES
        IMPORTED_IMPLIB
        $<IF:
            $<BOOL:${MTASA_X64}>
            , "${VENDOR_MYSQL_DIR}/lib/x64/libmysql.lib"
            , "${VENDOR_MYSQL_DIR}/lib/x86/libmysql.lib"
        >

        IMPORTED_LOCATION
        $<IF:
            $<BOOL:${MTASA_X64}>
            , "${VENDOR_MYSQL_DIR}/lib/x64/libmysql.dll"
            , "${VENDOR_MYSQL_DIR}/lib/x86/libmysql.dll"
        >
    )
endif()

target_include_directories(vendor-mysql INTERFACE "${VENDOR_MYSQL_DIR}/include")
