include(ExternalProject)

macro(download_network_library)
    if(MTASA_PLATFORM_WINDOWS)
        if(MTASA_ARCH_64_BIT)
            set(NETWORK_SERVER_DLL_URL "https://mirror.mtasa.com/bdata/net_64.dll")
            set(NETWORK_SERVER_DLL "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/server/x64/net.dll")
            # TODO: https://mirror.mtasa.com/bdata/netc.dll
        else()
            set(NETWORK_SERVER_DLL_URL "https://mirror.mtasa.com/bdata/net.dll")
            set(NETWORK_SERVER_DLL "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/server/net.dll")
        endif()

        if(NOT EXISTS ${NETWORK_SERVER_DLL})
            file(DOWNLOAD
                ${NETWORK_SERVER_DLL_URL}
                ${NETWORK_SERVER_DLL} 
                SHOW_PROGRESS
                TLS_VERIFY ON)
        endif()
    elseif(MTASA_PLATFORM_LINUX)
        # TODO: "https://mirror.mtasa.com/bdata/net.so"
        # TODO: "https://mirror.mtasa.com/bdata/net_64.so"
    elseif(MTASA_PLATFORM_APPLE)
        # TODO: "https://mirror.mtasa.com/bdata/net.dylib"
    endif()
endmacro()
