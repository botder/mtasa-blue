#
# PCRE library target
#
set(VENDOR_PCRE_DIR "${MTASA_VENDOR_DIR}/pcre")

add_library(vendor-pcre STATIC
    "${VENDOR_PCRE_DIR}/config.h"
    "${VENDOR_PCRE_DIR}/pcre.h"
    "${VENDOR_PCRE_DIR}/pcrecpp.cc"
    "${VENDOR_PCRE_DIR}/pcrecpp.h"
    "${VENDOR_PCRE_DIR}/pcrecpparg.h"
    "${VENDOR_PCRE_DIR}/pcrecpp_internal.h"
    "${VENDOR_PCRE_DIR}/pcregrep.c"
    "${VENDOR_PCRE_DIR}/pcreposix.c"
    "${VENDOR_PCRE_DIR}/pcreposix.h"
    "${VENDOR_PCRE_DIR}/pcre_byte_order.c"
    "${VENDOR_PCRE_DIR}/pcre_chartables.c"
    "${VENDOR_PCRE_DIR}/pcre_compile.c"
    "${VENDOR_PCRE_DIR}/pcre_config.c"
    "${VENDOR_PCRE_DIR}/pcre_dfa_exec.c"
    "${VENDOR_PCRE_DIR}/pcre_exec.c"
    "${VENDOR_PCRE_DIR}/pcre_fullinfo.c"
    "${VENDOR_PCRE_DIR}/pcre_get.c"
    "${VENDOR_PCRE_DIR}/pcre_globals.c"
    "${VENDOR_PCRE_DIR}/pcre_internal.h"
    "${VENDOR_PCRE_DIR}/pcre_jit_compile.c"
    "${VENDOR_PCRE_DIR}/pcre_maketables.c"
    "${VENDOR_PCRE_DIR}/pcre_newline.c"
    "${VENDOR_PCRE_DIR}/pcre_ord2utf8.c"
    "${VENDOR_PCRE_DIR}/pcre_printint.c"
    "${VENDOR_PCRE_DIR}/pcre_refcount.c"
    "${VENDOR_PCRE_DIR}/pcre_scanner.cc"
    "${VENDOR_PCRE_DIR}/pcre_scanner.h"
    "${VENDOR_PCRE_DIR}/pcre_stringpiece.cc"
    "${VENDOR_PCRE_DIR}/pcre_stringpiece.h"
    "${VENDOR_PCRE_DIR}/pcre_string_utils.c"
    "${VENDOR_PCRE_DIR}/pcre_study.c"
    "${VENDOR_PCRE_DIR}/pcre_tables.c"
    "${VENDOR_PCRE_DIR}/pcre_ucd.c"
    "${VENDOR_PCRE_DIR}/pcre_valid_utf8.c"
    "${VENDOR_PCRE_DIR}/pcre_version.c"
    "${VENDOR_PCRE_DIR}/pcre_xclass.c"
    "${VENDOR_PCRE_DIR}/ucp.h"
)

target_include_directories(vendor-pcre PUBLIC "${VENDOR_PCRE_DIR}")

target_compile_definitions(vendor-pcre PUBLIC
    HAVE_STRTOLL
    PCRE_STATIC
    HAVE_CONFIG_H
)
