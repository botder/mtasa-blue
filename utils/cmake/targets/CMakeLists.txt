#
# Predefine path variables for targets
#
set(MTASA_CLIENT_DIR "${MTASA_SOURCE_DIR}/Client")
set(MTASA_SERVER_DIR "${MTASA_SOURCE_DIR}/Server")
set(MTASA_SHARED_DIR "${MTASA_SOURCE_DIR}/Shared")
set(MTASA_VENDOR_DIR "${MTASA_SOURCE_DIR}/vendor")

if (MTASA_X64)
    set(MTASA_SERVER_OUTPUT_DIR "server/x64")
    set(MTASA_SERVER_MOD_OUTPUT_DIR "server/x64")
else()
    set(MTASA_SERVER_OUTPUT_DIR "server")
    set(MTASA_SERVER_MOD_OUTPUT_DIR "server/mods/deathmatch")
endif()

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
include(vendor-sqlite.cmake)
include(vendor-tinygettext.cmake)
include(vendor-tinyxml.cmake)
include(vendor-unrar.cmake)
include(vendor-zip.cmake)
include(vendor-zlib.cmake)
include(vendor-d3dx9.cmake)

#
# Shared targets
#
include(mtasa-shared-sdk.cmake)
include(mtasa-public-sdk.cmake)

#
# Client targets
#
if (MTASA_OS_WINDOWS AND MTASA_X86)
    include(mtasa-client-sdk.cmake)
    include(mtasa-client-multiplayer.cmake)
    include(mtasa-client-ceflauncher.cmake)
    include(mtasa-client-ceflauncher-dll.cmake)
    include(mtasa-client-cefweb.cmake)
    include(mtasa-client-launcher.cmake)
    include(mtasa-client-loader.cmake)
    include(mtasa-client-game.cmake)
    include(mtasa-client-core.cmake)
    include(mtasa-client-pthread.cmake)
    include(mtasa-client-cgui.cmake)
    include(mtasa-client-lua.cmake)
    include(mtasa-client-pcre.cmake)
    include(mtasa-client-xml.cmake)
    include(mtasa-client-deathmatch.cmake)
endif()

#
# Server targets
#
include(mtasa-server-launcher.cmake)
include(mtasa-server-core.cmake)
include(mtasa-server-sdk.cmake)
include(mtasa-server-pthread.cmake)
include(mtasa-server-xml.cmake)
include(mtasa-server-dbconmy.cmake)
include(mtasa-server-lua.cmake)
include(mtasa-server-pcre.cmake)
include(mtasa-server-deathmatch.cmake)
