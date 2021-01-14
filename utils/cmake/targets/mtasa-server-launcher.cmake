#
# Server launcher executable target
#
set(SERVER_LAUNCHER_DIR "${MTASA_SERVER_DIR}/launcher")

add_executable(mtasa-server-launcher
    "${SERVER_LAUNCHER_DIR}/CDynamicLibrary.cpp"
    "${SERVER_LAUNCHER_DIR}/CDynamicLibrary.h"
    "${SERVER_LAUNCHER_DIR}/Main.cpp"

    # $<$<BOOL:${MTASA_MSVC_NO_FRONTEND}>:
    #     "${SERVER_LAUNCHER_DIR}/resource.h"
    #     "${SERVER_LAUNCHER_DIR}/launcher.rc"
    # >
)

set_target_properties(mtasa-server-launcher PROPERTIES
    DEBUG_POSTFIX
    "${CMAKE_DEBUG_POSTFIX}"
)

set(SERVER_OUTPUT_NAME "mta-server")

if (MTASA_OS_WINDOWS)
    set(SERVER_OUTPUT_NAME "MTA Server")
endif()

if (MTASA_X64 AND (MTASA_OS_WINDOWS OR MTASA_OS_LINUX))
    set(SERVER_OUTPUT_NAME "${SERVER_OUTPUT_NAME}64")
endif()

set_property(TARGET mtasa-server-launcher PROPERTY OUTPUT_NAME "${SERVER_OUTPUT_NAME}")

mtasa_set_target_outputdir(mtasa-server-launcher "server")

target_link_libraries(mtasa-server-launcher PRIVATE
    mtasa-server-sdk
    mtasa-shared-sdk

    # $<$<BOOL:${MTASA_GCC}>:
    #     -static-libgcc -static-libstdc++ -static
    # >
)

target_compile_definitions(mtasa-server-launcher PRIVATE
    $<$<CONFIG:Debug>:MTA_DEBUG>
)

target_compile_options(mtasa-server-launcher PRIVATE
    $<$<BOOL:${MTASA_MSVC}>:
        /W3
    >

    # $<$<BOOL:${MTASA_GCC}>:
    #     -Wall -Wextra -pedantic
    # >
)
