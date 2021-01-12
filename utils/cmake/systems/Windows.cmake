#
# Windows operating system
#
add_compile_definitions(
    WIN32
    _WIN32
    _TIMESPEC_DEFINED
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    $<$<CONFIG:Debug>:DEBUG>

    # Windows 7 (_WIN32_WINNT_WIN7)
    # See https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160
    _WIN32_WINNT=0x601
)
