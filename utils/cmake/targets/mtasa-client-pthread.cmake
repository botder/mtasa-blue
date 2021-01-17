#
# Server pthread library target
#
add_library(mtasa-client-pthread SHARED
    $<TARGET_PROPERTY:vendor-pthread,SOURCES>
)

target_include_directories(mtasa-client-pthread PUBLIC
    $<TARGET_PROPERTY:vendor-pthread,INCLUDE_DIRECTORIES>
)

target_compile_definitions(mtasa-client-pthread PUBLIC
    _TIMESPEC_DEFINED
    HAVE_PTW32_CONFIG_H
    PTW32_BUILD_INLINED
)

set_target_properties(mtasa-client-pthread PROPERTIES
    OUTPUT_NAME "pthread"
    DEBUG_POSTFIX ""
)

mtasa_set_target_outputdir(mtasa-client-pthread "mta")

mtasa_create_debug_postfix_copy(mtasa-client-pthread)
