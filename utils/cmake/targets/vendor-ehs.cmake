#
# ehs library target
#
set(VENDOR_EHS_DIR "${MTASA_VENDOR_DIR}/ehs")

file(GLOB_RECURSE EHS_HEADERS LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${VENDOR_EHS_DIR}/*.h"
)

add_library(vendor-ehs STATIC
    ${EHS_HEADERS}
    "${VENDOR_EHS_DIR}/datum.cpp"
    "${VENDOR_EHS_DIR}/ehs.cpp"
    "${VENDOR_EHS_DIR}/httprequest.cpp"
    "${VENDOR_EHS_DIR}/httpresponse.cpp"
    "${VENDOR_EHS_DIR}/socket.cpp"
)

target_compile_definitions(vendor-ehs PUBLIC _LIB)

target_include_directories(vendor-ehs PUBLIC "${MTASA_VENDOR_DIR}")

target_link_libraries(vendor-ehs PRIVATE
    mtasa-shared-sdk
    vendor-pme
    vendor-pthread
)
