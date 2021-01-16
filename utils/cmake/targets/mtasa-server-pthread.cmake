#
# Server pthread library target
#
if (NOT MTASA_OS_WINDOWS)
    return()
endif()

set(VENDOR_PTHREAD_DIR "${MTASA_VENDOR_DIR}/pthreads")

add_library(mtasa-server-pthread SHARED
    "${VENDOR_PTHREAD_DIR}/include/pthread.c"
    "${VENDOR_PTHREAD_DIR}/include/pthread.h"
    "${VENDOR_PTHREAD_DIR}/include/config.h"
    "${VENDOR_PTHREAD_DIR}/include/context.h"
    "${VENDOR_PTHREAD_DIR}/include/implement.h"
    "${VENDOR_PTHREAD_DIR}/include/need_errno.h"
    "${VENDOR_PTHREAD_DIR}/include/sched.h"
    "${VENDOR_PTHREAD_DIR}/include/semaphore.h"
)

target_include_directories(mtasa-server-pthread PUBLIC "${VENDOR_PTHREAD_DIR}/include")

target_compile_definitions(mtasa-server-pthread PUBLIC
    _TIMESPEC_DEFINED
    HAVE_PTW32_CONFIG_H
    PTW32_BUILD_INLINED
)

set_target_properties(mtasa-server-pthread PROPERTIES
    OUTPUT_NAME "pthread"
    DEBUG_POSTFIX ""
)

mtasa_set_target_outputdir(mtasa-server-pthread "${MTASA_SERVER_OUTPUT_DIR}")

mtasa_create_debug_postfix_copy(mtasa-server-pthread)
