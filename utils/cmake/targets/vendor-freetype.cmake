#
# FreeType library target
#
set(VENDOR_FREETYPE_DIR "${MTASA_VENDOR_DIR}/freetype")

file(GLOB_RECURSE FREETYPE_HEADERS LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${VENDOR_FREETYPE_DIR}/include/*.h"
)

add_library(vendor-freetype STATIC
    ${FREETYPE_HEADERS}
    "${VENDOR_FREETYPE_DIR}/src/base/ftbbox.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftbdf.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftbitmap.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftcid.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftfstype.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftgasp.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftglyph.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftgxval.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftmm.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftotval.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftpatent.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftpfr.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftstroke.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftsynth.c"
    "${VENDOR_FREETYPE_DIR}/src/base/fttype1.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftwinfnt.c"
    "${VENDOR_FREETYPE_DIR}/src/autofit/autofit.c"
    "${VENDOR_FREETYPE_DIR}/src/bdf/bdf.c"
    "${VENDOR_FREETYPE_DIR}/src/cff/cff.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftbase.c"
    "${VENDOR_FREETYPE_DIR}/src/cache/ftcache.c"
    "${VENDOR_FREETYPE_DIR}/src/gzip/ftgzip.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftinit.c"
    "${VENDOR_FREETYPE_DIR}/src/lzw/ftlzw.c"
    "${VENDOR_FREETYPE_DIR}/src/base/ftsystem.c"
    "${VENDOR_FREETYPE_DIR}/src/pcf/pcf.c"
    "${VENDOR_FREETYPE_DIR}/src/pfr/pfr.c"
    "${VENDOR_FREETYPE_DIR}/src/psaux/psaux.c"
    "${VENDOR_FREETYPE_DIR}/src/pshinter/pshinter.c"
    "${VENDOR_FREETYPE_DIR}/src/psnames/psmodule.c"
    "${VENDOR_FREETYPE_DIR}/src/raster/raster.c"
    "${VENDOR_FREETYPE_DIR}/src/sfnt/sfnt.c"
    "${VENDOR_FREETYPE_DIR}/src/smooth/smooth.c"
    "${VENDOR_FREETYPE_DIR}/src/truetype/truetype.c"
    "${VENDOR_FREETYPE_DIR}/src/type1/type1.c"
    "${VENDOR_FREETYPE_DIR}/src/cid/type1cid.c"
    "${VENDOR_FREETYPE_DIR}/src/type42/type42.c"
    "${VENDOR_FREETYPE_DIR}/src/winfonts/winfnt.c"
    "${VENDOR_FREETYPE_DIR}/builds/windows/ftdebug.c"
)

target_include_directories(vendor-freetype PUBLIC "${VENDOR_FREETYPE_DIR}/include")

target_compile_definitions(vendor-freetype
    PRIVATE
        UNICODE
        _UNICODE

        $<$<CONFIG:Debug>:
            FT_DEBUG_LEVEL_ERROR
            FT_DEBUG_LEVEL_TRACE
        >
    PUBLIC
        FT2_BUILD_LIBRARY
)

target_link_libraries(vendor-freetype PUBLIC
    vendor-zlib
    vendor-png
)
