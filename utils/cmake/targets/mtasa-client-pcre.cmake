#
# Client PCRE library target
#
add_library(mtasa-client-pcre SHARED $<TARGET_PROPERTY:vendor-pcre,SOURCES>)

target_include_directories(mtasa-client-pcre PUBLIC
    $<TARGET_PROPERTY:vendor-pcre,INCLUDE_DIRECTORIES>
)

target_compile_definitions(mtasa-client-pcre PUBLIC
    HAVE_CONFIG_H
)

set_target_properties(mtasa-client-pcre PROPERTIES OUTPUT_NAME "pcre3")

mtasa_set_target_outputdir(mtasa-client-pcre "mods/deathmatch")
