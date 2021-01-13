#
# Predefine path variables for targets
#
set(MTASA_CLIENT_DIR "${MTASA_SOURCE_DIR}/Client")
set(MTASA_SERVER_DIR "${MTASA_SOURCE_DIR}/Server")
set(MTASA_SHARED_DIR "${MTASA_SOURCE_DIR}/Shared")
set(MTASA_VENDOR_DIR "${MTASA_SOURCE_DIR}/vendor")

#
# Client targets
#
# TODO

#
# Server targets
#
# TODO

#
# Shared targets
#
include(mtasa-shared-sdk.cmake)

#
# Vendor targets
#
include(vendor-pthread.cmake)
include(vendor-bass.cmake)
include(vendor-bcrypt.cmake)
include(vendor-cef.cmake)
include(vendor-cegui.cmake)
include(vendor-cryptopp.cmake)
include(vendor-freetype.cmake)
include(vendor-curl.cmake)
include(vendor-lua.cmake)
include(vendor-zlib.cmake)
include(vendor-mbedtls.cmake)
include(vendor-detours.cmake)
include(vendor-breakpad.cmake)
include(vendor-pme.cmake)
include(vendor-pcre.cmake)
include(vendor-ehs.cmake)
include(vendor-hwbrk.cmake)
include(vendor-jpeg.cmake)
include(vendor-json.cmake)
include(vendor-ksignals.cmake)
include(vendor-png.cmake)
include(vendor-speex.cmake)
include(vendor-mysql.cmake)
include(vendor-nvapi.cmake)
include(vendor-portaudio.cmake)
include(vendor-sparsehash.cmake)
