#
# tinyxml library target
#
set(VENDOR_TINYXML_DIR "${MTASA_VENDOR_DIR}/tinyxml")

add_library(vendor-tinyxml STATIC
    "${VENDOR_TINYXML_DIR}/tinystr.cpp"
    "${VENDOR_TINYXML_DIR}/tinyxml.cpp"
    "${VENDOR_TINYXML_DIR}/tinyxmlerror.cpp"
    "${VENDOR_TINYXML_DIR}/tinyxmlparser.cpp"
)

target_compile_definitions(vendor-tinyxml PUBLIC TIXML_USE_STL)

target_include_directories(vendor-tinyxml PUBLIC "${VENDOR_TINYXML_DIR}")

target_link_libraries(vendor-tinyxml PRIVATE mtasa-shared-sdk)
