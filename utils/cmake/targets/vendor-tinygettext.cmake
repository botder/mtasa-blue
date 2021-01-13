#
# tinygettext library target
#
if (NOT MTASA_X86)
    return()
endif()

set(VENDOR_TINYGETTEXT_DIR "${MTASA_VENDOR_DIR}/tinygettext")

add_library(vendor-tinygettext STATIC
    "${VENDOR_TINYGETTEXT_DIR}/dictionary.cpp"
    "${VENDOR_TINYGETTEXT_DIR}/dictionary.hpp"
    "${VENDOR_TINYGETTEXT_DIR}/dictionary_manager.cpp"
    "${VENDOR_TINYGETTEXT_DIR}/dictionary_manager.hpp"
    "${VENDOR_TINYGETTEXT_DIR}/dirent_win32.h"
    "${VENDOR_TINYGETTEXT_DIR}/file_system.hpp"
    "${VENDOR_TINYGETTEXT_DIR}/language.cpp"
    "${VENDOR_TINYGETTEXT_DIR}/language.hpp"
    "${VENDOR_TINYGETTEXT_DIR}/log.cpp"
    "${VENDOR_TINYGETTEXT_DIR}/log.hpp"
    "${VENDOR_TINYGETTEXT_DIR}/log_stream.hpp"
    "${VENDOR_TINYGETTEXT_DIR}/plural_forms.cpp"
    "${VENDOR_TINYGETTEXT_DIR}/plural_forms.hpp"
    "${VENDOR_TINYGETTEXT_DIR}/po_parser.cpp"
    "${VENDOR_TINYGETTEXT_DIR}/po_parser.hpp"
    "${VENDOR_TINYGETTEXT_DIR}/tinygettext.cpp"
    "${VENDOR_TINYGETTEXT_DIR}/tinygettext.hpp"
    "${VENDOR_TINYGETTEXT_DIR}/unix_file_system.cpp"
    "${VENDOR_TINYGETTEXT_DIR}/unix_file_system.hpp"
)

target_include_directories(vendor-tinygettext PUBLIC "${VENDOR_TINYGETTEXT_DIR}")

target_link_libraries(vendor-tinygettext PRIVATE mtasa-shared-sdk)
