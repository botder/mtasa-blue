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
# Vendor targets
#
include(vendor-bass.cmake)
include(vendor-bcrypt.cmake)
include(vendor-lua.cmake)
