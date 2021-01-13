#
# BASS library targets
#
set(VENDOR_BASS_DIR "${MTASA_VENDOR_DIR}/bass")

add_library(vendor-bass-sdk INTERFACE)
target_include_directories(vendor-bass-sdk INTERFACE "${VENDOR_BASS_DIR}")

add_library(vendor-bass STATIC IMPORTED)
set_property(TARGET vendor-bass PROPERTY IMPORTED_IMPLIB "${VENDOR_BASS_DIR}/lib/bass.lib")

add_library(vendor-bass-fx STATIC IMPORTED)
set_property(TARGET vendor-bass-fx PROPERTY IMPORTED_IMPLIB "${VENDOR_BASS_DIR}/lib/bass_fx.lib")

add_library(vendor-bass-mix STATIC IMPORTED)
set_property(TARGET vendor-bass-mix PROPERTY IMPORTED_IMPLIB "${VENDOR_BASS_DIR}/lib/bassmix.lib")

add_library(vendor-bass-tags STATIC IMPORTED)
set_property(TARGET vendor-bass-tags PROPERTY IMPORTED_IMPLIB "${VENDOR_BASS_DIR}/lib/tags.lib")
