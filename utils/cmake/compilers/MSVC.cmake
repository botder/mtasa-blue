#
# Microsoft Visual C++ compiler
#
set(MTASA_MSVC TRUE)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_compile_options(
    # See https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus
    # See https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
    /Zc:__cplusplus
)

add_compile_definitions(
    _CRT_SECURE_NO_WARNINGS
    _SCL_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_DEPRECATE
)
