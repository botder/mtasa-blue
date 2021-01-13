#
# pthread library target
#
set(VENDOR_PTHREAD_DIR "${MTASA_VENDOR_DIR}/pthreads")

add_library(vendor-pthread STATIC "${VENDOR_PTHREAD_DIR}/include/pthread.c")

target_include_directories(vendor-pthread PUBLIC "${VENDOR_PTHREAD_DIR}/include")

target_compile_definitions(vendor-pthread PUBLIC
    _TIMESPEC_DEFINED

    $<$<BOOL:${MTASA_OS_WINDOWS}>:
        HAVE_PTW32_CONFIG_H
        PTW32_BUILD_INLINED
    >
)
