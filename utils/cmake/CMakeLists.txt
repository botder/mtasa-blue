#
# Include compiler specific CMake files
# See https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
#
set(COMPILER_INCLUDE_FILE "compilers/${CMAKE_CXX_COMPILER_ID}.cmake")

if (EXISTS ${COMPILER_INCLUDE_FILE})
    include(${COMPILER_INCLUDE_FILE})
endif()

#
# Include generator specific CMake files
#
if (CMAKE_GENERATOR MATCHES "^Visual Studio")
    include(generators/VisualStudio.cmake)
endif()

#
# Detections and auxiliary CMake files
#
include(DetectTargetSystem.cmake)
include(Auxiliary.cmake)

#
# Targets built in this project
#
add_subdirectory(targets)
