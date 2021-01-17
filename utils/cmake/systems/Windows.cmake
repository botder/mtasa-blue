#
# Windows operating system
#
add_compile_definitions(
    WIN32
    _WIN32
    $<$<CONFIG:Debug>:DEBUG>

    # Windows 7 (_WIN32_WINNT_WIN7)
    # See https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160
    _WIN32_WINNT=0x601
    WINVER=0x601
)

#
# Remove NDEBUG compiler definition for release configurations
# (This is required for luaX_is_apicheck_enabled)
#
foreach (config MINSIZEREL RELEASE RELWITHDEBINFO)
    if (DEFINED "CMAKE_CXX_FLAGS_${config}")
        string(REPLACE "/DNDEBUG" "" "CMAKE_CXX_FLAGS_${config}" "${CMAKE_CXX_FLAGS_${config}}")
    endif()

    if (DEFINED "CMAKE_C_FLAGS_${config}")
        string(REPLACE "/DNDEBUG" "" "CMAKE_C_FLAGS_${config}" "${CMAKE_C_FLAGS_${config}}")
    endif()
endforeach()
