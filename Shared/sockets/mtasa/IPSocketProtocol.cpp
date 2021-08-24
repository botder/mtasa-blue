/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPSocketProtocol.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "IPSocketProtocol.h"

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

namespace mtasa
{
    std::pair<int, int> TranslateSocketProtocol(IPSocketProtocol protocol)
    {
        switch (protocol)
        {
            case IPSocketProtocol::TCP:
                return {SOCK_STREAM, IPPROTO_TCP};
            case IPSocketProtocol::UDP:
                return {SOCK_DGRAM, IPPROTO_UDP};
            default:
                break;
        }

        return {0, 0};
    }
}            // namespace mtasa
