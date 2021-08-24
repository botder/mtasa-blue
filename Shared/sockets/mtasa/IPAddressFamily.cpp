/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPAddressFamily.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "IPAddressFamily.h"

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
#endif

namespace mtasa
{
    int TranslateAddressFamily(IPAddressFamily addressFamily)
    {
        switch (addressFamily)
        {
            case IPAddressFamily::IPv4:
                return AF_INET;
            case IPAddressFamily::IPv6:
                return AF_INET6;
            case IPAddressFamily::Unspecified:
            default:
                break;
        }

        return 0;
    }
}            // namespace mtasa
