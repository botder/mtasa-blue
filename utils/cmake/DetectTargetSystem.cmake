#
# Compiles a simple C file to detect target system, architecture and endianness
# See https://sourceforge.net/p/predef/wiki/OperatingSystems/
# See https://sourceforge.net/p/predef/wiki/Architectures/
#
set(DetectTargetSystem_COMPILER_CODE "
#include <stdio.h>

#if defined(__APPLE__)
#   include <TargetConditionals.h>
#endif

#if    defined(_WIN32) \\
    || defined(_WIN64) \\
    || defined(__WIN32__) \\
    || defined(__CYGWIN__) \\
    || defined(TARGET_OS_WIN32)
#   define OS_NAME Windows
#elif defined(__ANDROID__)
#   define OS_NAME Android
#elif  defined(macintosh) \\
    || defined(Macintosh) \\
    || (defined(__APPLE__) && defined(__MACH__))
#   if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
#       define OS_NAME Apple iOS
#   elif TARGET_OS_MAC
#       define OS_NAME Apple OSX
#   else
#       define OS_NAME MacOS
#   endif
#elif  defined(__FreeBSD__) \\
    || defined(__NetBSD__) \\
    || defined(__OpenBSD__) \\
    || defined(__DragonFly__) \\
    || defined(__bsdi__) \\
    || defined(BSD)
#   define OS_NAME BSD
#elif defined(__linux__) || defined(__gnu_linux__)
#   define OS_NAME Linux
#else
#   define OS_NAME Unknown
#endif

#if   (defined(_M_IX86) && defined(_MSC_VER)) \\
    || defined(_M_I386) \\
    || defined(__X86__) \\
    || defined(_X86_) \\
    || defined(i386) \\
    || defined(__i386) \\
    || defined(__i386__) \\
    || defined(__i486__) \\
    || defined(__i586__) \\
    || defined(__i686__)
#   define OS_ARCH Intel x86
#elif  defined(_M_AMD64) \\
    || defined(__amd64__) \\
    || defined(__amd64) \\
    || defined(__x86_64__) \\
    || defined(__x86_64)
#   define OS_ARCH AMD64
#elif  defined(_M_PPC) \\
    || defined(__powerpc) \\
    || defined(__powerpc__) \\
    || defined(__powerpc64__) \\
    || defined(__POWERPC__) \\
    || defined(_ARCH_PPC)
#   define PowerPC
#elif  defined(__sparc__) \\
    || defined(__sparc) \\
    || defined(__sparc_v8__) \\
    || defined(__sparc_v9__) \\
    || defined(__sparcv8) \\
    || defined(__sparcv9)
#   define SPARC
#elif defined(__aarch64__)
#   define ARM64
#elif defined(__arm__)
#   define ARM
#else
#   define OS_ARCH Unknown
#endif

#define STR_EXPAND(x) #x
#define STR(x) STR_EXPAND(x)

int isLittleEndian() {
    int i = 1;
    return (int)*((unsigned char *)&i) == 1;
}

int main() {
    printf(\"%s;%s;%d\", STR(OS_NAME), STR(OS_ARCH), isLittleEndian());
    return 0;
}
")

set(DetectTargetSystem_COMPILER_FILE "${CMAKE_BINARY_DIR}/DetectTargetSystem.c")

function(DetectTargetSystem TargetSystemOutput TargetArchitectureOutput TargetEndiannessOutput)
    file(WRITE "${DetectTargetSystem_COMPILER_FILE}" "${DetectTargetSystem_COMPILER_CODE}")

    try_run(
        UNUSED_RUN_RESULT
        COMPILE_SUCCESS
        "${CMAKE_BINARY_DIR}"
        "${DetectTargetSystem_COMPILER_FILE}"
        COMPILE_OUTPUT_VARIABLE COMPILER_OUTPUT
        RUN_OUTPUT_VARIABLE TARGET_SYSTEM
    )

    if (NOT COMPILE_SUCCESS)
        message(FATAL_ERROR "Compiler error: ${COMPILER_OUTPUT}")
    endif()

    list(GET TARGET_SYSTEM 0 TARGET_OS)
    list(GET TARGET_SYSTEM 1 TARGET_ARCH)
    list(GET TARGET_SYSTEM 2 TARGET_ENDIANNESS)

    if (NOT TARGET_OS)
        set(TARGET_OS "Unknown")
    endif()

    if (NOT TARGET_ARCH)
        set(TARGET_ARCH "Unknown")
    endif()

    if (NOT TARGET_ENDIANNESS OR TARGET_ENDIANNESS STREQUAL "1")
        set(TARGET_ENDIANNESS "Little Endian")
    else()
        set(TARGET_ENDIANNESS "Big Endian")
    endif()

    set(${TargetSystemOutput} "${TARGET_OS}" PARENT_SCOPE)
    set(${TargetArchitectureOutput} "${TARGET_ARCH}" PARENT_SCOPE)
    set(${TargetEndiannessOutput} "${TARGET_ENDIANNESS}" PARENT_SCOPE)
endfunction()

DetectTargetSystem(MTASA_TARGET_OS MTASA_TARGET_ARCH MTASA_TARGET_ENDIANNESS)

#
# Process detection result and define auxiliary variables
#
# -> Endianness
if (MTASA_TARGET_ENDIANNESS STREQUAL "Little Endian")
    set(MTASA_LITTLE_ENDIAN TRUE)
elseif (MTASA_TARGET_ENDIANNESS STREQUAL "Big Endian")
    set(MTASA_BIG_ENDIAN TRUE)
endif()

# -> Architecture
if (MTASA_TARGET_ARCH STREQUAL "Intel x86")
    set(MTASA_X86 TRUE)
endif()

if (MTASA_TARGET_ARCH STREQUAL "AMD64" OR MTASA_TARGET_ARCH STREQUAL "ARM64")
    set(MTASA_X64 TRUE)
endif()

if (MTASA_TARGET_ARCH STREQUAL "ARM" OR MTASA_TARGET_ARCH STREQUAL "ARM64")
    set(MTASA_ARM TRUE)
endif()

# -> Operating system
if (MTASA_TARGET_OS STREQUAL "Windows")
    set(MTASA_OS_WINDOWS TRUE)
elseif (MTASA_TARGET_OS STREQUAL "Android")
    set(MTASA_OS_ANDROID TRUE)
elseif (MTASA_TARGET_OS STREQUAL "BSD")
    set(MTASA_OS_BSD TRUE)
elseif (MTASA_TARGET_OS STREQUAL "Linux")
    set(MTASA_OS_LINUX TRUE)
elseif (MTASA_TARGET_OS STREQUAL "MacOS")
    set(MTASA_OS_APPLE TRUE)
endif()

# -> Debug output for variables
message(DEBUG "OS         -> ${MTASA_TARGET_OS}")
message(DEBUG "ARCH       -> ${MTASA_TARGET_ARCH}")
message(DEBUG "ENDIANNESS -> ${MTASA_TARGET_ENDIANNESS}")

message(DEBUG "MTASA_LITTLE_ENDIAN -> ${MTASA_LITTLE_ENDIAN}")
message(DEBUG "MTASA_BIG_ENDIAN    -> ${MTASA_BIG_ENDIAN}")

message(DEBUG "MTASA_X86 -> ${MTASA_X86}")
message(DEBUG "MTASA_X64 -> ${MTASA_X64}")
message(DEBUG "MTASA_ARM -> ${MTASA_ARM}")

message(DEBUG "MTASA_OS_WINDOWS -> ${MTASA_OS_WINDOWS}")
message(DEBUG "MTASA_OS_ANDROID -> ${MTASA_OS_ANDROID}")
message(DEBUG "MTASA_OS_BSD     -> ${MTASA_OS_BSD}")
message(DEBUG "MTASA_OS_LINUX   -> ${MTASA_OS_LINUX}")
message(DEBUG "MTASA_OS_APPLE   -> ${MTASA_OS_APPLE}")
