#
# cURL library target
# See https://github.com/curl/curl/blob/master/CMakeLists.txt
#
set(VENDOR_CURL_DIR "${MTASA_VENDOR_DIR}/curl")

file(GLOB_RECURSE CURL_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS
    "${VENDOR_CURL_DIR}/lib/*.c"
    "${VENDOR_CURL_DIR}/lib/*.h"
)

add_library(vendor-curl STATIC ${CURL_SOURCES})

target_include_directories(vendor-curl PUBLIC
    "${VENDOR_CURL_DIR}/include"
    "${VENDOR_CURL_DIR}/lib"
)

target_link_libraries(vendor-curl PRIVATE
    vendor-zlib

    $<$<BOOL:${MTASA_OS_WINDOWS}>:
        vendor-mbedtls
        crypt32
        Normaliz
    >
)

target_compile_definitions(vendor-curl PRIVATE
    BUILDING_LIBCURL
    CURL_STATICLIB
    HTTP_ONLY
    USE_ZLIB
    HAVE_LIBZ
    HAVE_ZLIB_H
    HAVE_CONFIG_H

    $<$<BOOL:${MTASA_OS_WINDOWS}>:
        USE_SCHANNEL
        USE_WINDOWS_SSPI
        USE_WIN32_IDN
        WANT_IDN_PROTOTYPES
    >
    
    $<$<BOOL:${MTASA_OS_LINUX}>:
        USE_MBEDTLS
    >

    $<$<BOOL:${MTASA_OS_APPLE}>:
        USE_DARWINSSL
    >

    $<$<NOT:$<BOOL:${MTASA_OS_WINDOWS}>>:
        CURL_HIDDEN_SYMBOLS
    >
)

if (NOT MTASA_OS_WINDOWS)
    if (NOT CURL_CA_BUNDLE)
        set(SEARCH_CA_BUNDLE_PATHS
            /etc/ssl/certs/ca-certificates.crt
            /etc/pki/tls/certs/ca-bundle.crt
            /usr/share/ssl/certs/ca-bundle.crt
            /usr/local/share/certs/ca-root-nss.crt
            /etc/ssl/cert.pem
        )

        foreach (SEARCH_CA_BUNDLE_PATH ${SEARCH_CA_BUNDLE_PATHS})
            if (EXISTS "${SEARCH_CA_BUNDLE_PATH}")
                message(STATUS "Found CA bundle: ${SEARCH_CA_BUNDLE_PATH}")
                set(CURL_CA_BUNDLE "${SEARCH_CA_BUNDLE_PATH}")
                break()
            endif()
        endforeach()
    endif()

    if (CURL_CA_BUNDLE)
        target_compile_definitions(vendor-curl PRIVATE CURL_CA_BUNDLE="${CURL_CA_BUNDLE}")
    endif()
endif()
