#
# pme library target
#
set(VENDOR_PME_DIR "${MTASA_VENDOR_DIR}/pme")

add_library(vendor-pme STATIC "${VENDOR_PME_DIR}/pme.cpp")

target_include_directories(vendor-pme PUBLIC "${VENDOR_PME_DIR}")

target_link_libraries(vendor-pme PUBLIC vendor-pcre)
