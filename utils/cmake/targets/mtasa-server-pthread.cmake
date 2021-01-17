#
# Server pthread library target
#
if (NOT MTASA_OS_WINDOWS)
    return()
endif()

add_library(mtasa-server-pthread SHARED
    $<TARGET_PROPERTY:vendor-pthread,SOURCES>
)

target_include_directories(mtasa-server-pthread PUBLIC
    $<TARGET_PROPERTY:vendor-pthread,INCLUDE_DIRECTORIES>
)

target_compile_definitions(mtasa-server-pthread PUBLIC
    _TIMESPEC_DEFINED
    HAVE_PTW32_CONFIG_H
    PTW32_BUILD_INLINED
)

set_target_properties(mtasa-server-pthread PROPERTIES
    OUTPUT_NAME "pthread"
    DEBUG_POSTFIX ""
)

mtasa_set_target_outputdir(mtasa-server-pthread "${MTASA_SERVER_OUTPUT_DIR}")

mtasa_create_debug_postfix_copy(mtasa-server-pthread)
