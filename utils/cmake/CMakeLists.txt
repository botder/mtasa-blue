#
# Detections and auxiliary CMake files
#
include(DetectTargetSystem.cmake)
include(Auxiliary.cmake)
include(OutputDirectory.cmake)

#
# Project wide setup
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED NO)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_POSITION_INDEPENDENT_CODE YES)
set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_UNITY_BUILD NO)

#
# Include compiler specific CMake file
# See https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
#
set(COMPILER_INCLUDE_FILE "${CMAKE_CURRENT_LIST_DIR}/compilers/${CMAKE_CXX_COMPILER_ID}.cmake")

if (EXISTS ${COMPILER_INCLUDE_FILE})
    include(${COMPILER_INCLUDE_FILE})
endif()

#
# Include generator specific CMake file
#
if (CMAKE_GENERATOR MATCHES "^Visual Studio")
    include(generators/VisualStudio.cmake)
endif()

#
# Include target system specific CMake file
#
set(TARGET_SYSTEM_INCLUDE_FILE "${CMAKE_CURRENT_LIST_DIR}/systems/${MTASA_TARGET_OS}.cmake")

if (EXISTS ${TARGET_SYSTEM_INCLUDE_FILE})
    include(${TARGET_SYSTEM_INCLUDE_FILE})
endif()

#
# Targets built in this project
#
add_subdirectory(targets)
