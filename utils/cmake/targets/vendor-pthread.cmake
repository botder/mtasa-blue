#
# pthread library target
#
if (NOT MTASA_OS_WINDOWS)
    return()
endif()

set(VENDOR_PTHREAD_DIR "${MTASA_VENDOR_DIR}/pthreads")

add_library(vendor-pthread STATIC
    "${VENDOR_PTHREAD_DIR}/include/pthread.c"
    "${VENDOR_PTHREAD_DIR}/include/pthread.h"
    "${VENDOR_PTHREAD_DIR}/include/config.h"
    "${VENDOR_PTHREAD_DIR}/include/context.h"
    "${VENDOR_PTHREAD_DIR}/include/implement.h"
    "${VENDOR_PTHREAD_DIR}/include/need_errno.h"
    "${VENDOR_PTHREAD_DIR}/include/sched.h"
    "${VENDOR_PTHREAD_DIR}/include/semaphore.h"
)

target_include_directories(vendor-pthread PUBLIC "${VENDOR_PTHREAD_DIR}/include")

target_compile_definitions(vendor-pthread
    PRIVATE
        PTW32_STATIC_LIB

    PUBLIC
        _TIMESPEC_DEFINED
        HAVE_PTW32_CONFIG_H
        PTW32_BUILD_INLINED
)
