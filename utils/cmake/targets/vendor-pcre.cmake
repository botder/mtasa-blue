#
# PCRE library target
#
set(VENDOR_PCRE_DIR "${MTASA_VENDOR_DIR}/pcre")

file(GLOB_RECURSE PCRE_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${VENDOR_PCRE_DIR}/*.c"
    "${VENDOR_PCRE_DIR}/*.cc"
    "${VENDOR_PCRE_DIR}/*.h"
)

add_library(vendor-pcre STATIC ${PCRE_SOURCES})

# pcrecpp.cc
# pcregrep.c
# pcreposix.c
# pcre_byte_order.c
# pcre_chartables.c
# pcre_compile.c
# pcre_config.c
# pcre_dfa_exec.c
# pcre_exec.c
# pcre_fullinfo.c
# pcre_get.c
# pcre_globals.c
# pcre_jit_compile.c
# pcre_maketables.c
# pcre_newline.c
# pcre_ord2utf8.c
# pcre_printint.c
# pcre_refcount.c
# pcre_scanner.cc
# pcre_stringpiece.cc
# pcre_string_utils.c
# pcre_study.c
# pcre_tables.c
# pcre_ucd.c
# pcre_valid_utf8.c
# pcre_version.c
# pcre_xclass.c

target_include_directories(vendor-pcre PUBLIC "${VENDOR_PCRE_DIR}")

target_compile_definitions(vendor-pcre PUBLIC HAVE_CONFIG_H)
