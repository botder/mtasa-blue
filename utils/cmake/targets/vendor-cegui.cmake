#
# CEGUI library target
#
if (NOT (MTASA_OS_WINDOWS AND MTASA_X86))
    return()
endif()

set(VENDOR_CEGUI_DIR "${MTASA_VENDOR_DIR}/cegui-0.4.0-custom")

# -> Source files
file(GLOB CEGUI_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${VENDOR_CEGUI_DIR}/src/elements/*.cpp"
    "${VENDOR_CEGUI_DIR}/src/falagard/*.cpp"
    "${VENDOR_CEGUI_DIR}/src/tinyxml/*.cpp"
    "${VENDOR_CEGUI_DIR}/src/*.cpp"
)

list(FILTER CEGUI_SOURCES EXCLUDE REGEX "StdInc.cpp$")

# -> Header files
file(GLOB_RECURSE CEGUI_HEADERS LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${VENDOR_CEGUI_DIR}/include/*.h"
    "${VENDOR_CEGUI_DIR}/src/pcre/*.h"
)

list(FILTER CEGUI_HEADERS EXCLUDE REGEX "StdInc.h$")

# -> Target definition
add_library(vendor-cegui STATIC ${CEGUI_SOURCES} ${CEGUI_HEADERS})

target_precompile_headers(vendor-cegui PRIVATE
    "${VENDOR_CEGUI_DIR}/include/StdInc.h"
)

target_include_directories(vendor-cegui PRIVATE
    "${VENDOR_CEGUI_DIR}/dependencies/include"
    "${VENDOR_CEGUI_DIR}/include"
)

target_link_libraries(vendor-cegui PRIVATE
    vendor-cegui-pcre
    vendor-freetype
)

target_compile_definitions(vendor-cegui PRIVATE
    CEGUIBASE_EXPORTS
    _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
)

#
# Custom dependency targets for CEGUI
#
add_library(vendor-cegui-pcre STATIC
    "${VENDOR_CEGUI_DIR}/src/pcre/chartables.c"
    "${VENDOR_CEGUI_DIR}/src/pcre/get.c"
    "${VENDOR_CEGUI_DIR}/src/pcre/maketables.c"
    "${VENDOR_CEGUI_DIR}/src/pcre/pcre.c"
    "${VENDOR_CEGUI_DIR}/src/pcre/pcreposix.c"
    "${VENDOR_CEGUI_DIR}/src/pcre/study.c"
)
