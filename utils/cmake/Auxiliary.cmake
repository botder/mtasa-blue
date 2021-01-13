#
# Overwrites the output directory for the target, which might be otherwise
# malformed by a multi-configuration generator like Visual Studio, which appens
# the configuration name to the output directory
#
function(mtasa_target_set_outputdir TargetName OutputDirectory)
    if (TARGET ${TargetName})
        foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${CONFIG} CONFIG)

            set_target_properties(${TargetName} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${OutputDirectory}"
                LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${OutputDirectory}"
                ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${OutputDirectory}"
            )
        endforeach()
    endif()
endfunction()

#
# Enables the permissive-flag (standards conformance), if compiled with a MSVC compiler
#
function(mtasa_target_enable_msvc_permissive TargetName)
    if (MTASA_MSVC)
        target_compile_options(${TargetName} PRIVATE "/permissive-")
    endif()
endfunction()

#
# Applies per-monitor DPI Awareness in the Visual Studio target project properties
#
function(mtasa_target_set_dpi_aware TargetName)
    if (MTASA_VISUALSTUDIO)
        set_property(TARGET ${TargetName} PROPERTY VS_DPI_AWARE "PerMonitor")
    endif()
endfunction()

#
# Creates a shared library with the object library target
#
macro(mtasa_add_shared_library TargetName ObjectTargetName)
    add_library("${TargetName}" SHARED $<TARGET_OBJECTS:${ObjectTargetName}>)
    target_include_directories("${TargetName}" PUBLIC $<TARGET_PROPERTY:${ObjectTargetName},INTERFACE_INCLUDE_DIRECTORIES>)
endmacro()

#
# Creates a static library with the object library target
#
macro(mtasa_add_static_library TargetName ObjectTargetName)
    add_library("${TargetName}" STATIC $<TARGET_OBJECTS:${ObjectTargetName}>)
    target_include_directories("${TargetName}" PUBLIC $<TARGET_PROPERTY:${ObjectTargetName},INTERFACE_INCLUDE_DIRECTORIES>)
endmacro()

#
# Links the object library target to the given target
#
macro(mtasa_link_object_library TargetName ObjectTargetName)
    target_sources("${TargetName}" PRIVATE $<TARGET_OBJECTS:${ObjectTargetName}>)
    target_include_directories("${TargetName}" PRIVATE $<TARGET_PROPERTY:${ObjectTargetName},INTERFACE_INCLUDE_DIRECTORIES>)
    target_compile_definitions("${TargetName}" PRIVATE $<TARGET_PROPERTY:${ObjectTargetName},INTERFACE_COMPILE_DEFINITIONS>)
endmacro()
