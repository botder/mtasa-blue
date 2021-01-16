#
# MySQL library target
#
set(VENDOR_MYSQL_DIR "${MTASA_VENDOR_DIR}/mysql")

add_library(vendor-mysql SHARED IMPORTED)

if (MTASA_OS_WINDOWS)
    if (MTASA_X64)
        set_target_properties(vendor-mysql PROPERTIES
            IMPORTED_IMPLIB
            "${VENDOR_MYSQL_DIR}/lib/x64/libmysql.lib"
            IMPORTED_LOCATION
            "${VENDOR_MYSQL_DIR}/lib/x64/libmysql.dll"
        )
    else()
        set_target_properties(vendor-mysql PROPERTIES
            IMPORTED_IMPLIB
            "${VENDOR_MYSQL_DIR}/lib/x86/libmysql.lib"
            IMPORTED_LOCATION
            "${VENDOR_MYSQL_DIR}/lib/x86/libmysql.dll"
        )
    endif()
endif()

target_include_directories(vendor-mysql INTERFACE "${VENDOR_MYSQL_DIR}/include")
