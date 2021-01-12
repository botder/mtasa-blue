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
    if (TARGET ${TargetName} AND MTASA_MSVC)
        target_compile_options(${TargetName} PRIVATE "/permissive-")
    endif()
endfunction()

#
# Applies per-monitor DPI Awareness in the Visual Studio target project properties
#
function(mtasa_target_set_dpi_aware TargetName)
    if (TARGET ${TargetName} AND MTASA_VISUALSTUDIO)
        set_property(TARGET ${TargetName} PROPERTY VS_DPI_AWARE "PerMonitor")
    endif()
endfunction()
