#
# Microsoft Visual C++ compiler
#
set(MTASA_MSVC TRUE)
# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(MTASA_MSVC_FRONTEND TRUE)
else()
    set(MTASA_MSVC_NO_FRONTEND TRUE)
endif()

add_compile_definitions(
    _CRT_SECURE_NO_WARNINGS
    _SCL_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_DEPRECATE
)

# See https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus
# See https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
list(APPEND MTASA_COMPILE_OPTIONS "/Zc:__cplusplus")
