/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sdk/net/IPEndPoint.hpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

// The code below requires at least C++17 support
#if __cplusplus >= 201703L

#include "IPEndPoint.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#endif

static_assert(sizeof(sockaddr_in::sin_addr) == 4, "Invalid size of sockaddr_in::sin_addr");
static_assert(sizeof(sockaddr_in6::sin6_addr) == 16, "Invalid size of sockaddr_in6::sin6_addr");

bool mtasa::IPEndPoint::ToSocketAddress(sockaddr& address, std::size_t& addressSize) const
{
    if (!m_port)
        return false;

    switch (m_address.GetAddressFamily())
    {
        case IPAddressFamily::IPv4:
        {
            auto maybeBytes = m_address.GetIPv4Bytes();

            if (maybeBytes && addressSize >= sizeof(sockaddr_in))
            {
                addressSize = sizeof(sockaddr_in);

                auto& ipv4 = reinterpret_cast<sockaddr_in&>(address);
                ipv4.sin_family = AF_INET;
                ipv4.sin_port = GetNetworkByteOrderPort();
                std::copy(maybeBytes->begin(), maybeBytes->end(), reinterpret_cast<std::uint8_t*>(&ipv4.sin_addr));
                return true;
            }

            break;
        }
        case IPAddressFamily::IPv6:
        {
            auto maybeBytes = m_address.GetIPv6Bytes();

            if (maybeBytes && addressSize >= sizeof(sockaddr_in6))
            {
                addressSize = sizeof(sockaddr_in6);

                auto& ipv6 = reinterpret_cast<sockaddr_in6&>(address);
                ipv6.sin6_family = AF_INET6;
                ipv6.sin6_port = GetNetworkByteOrderPort();
                std::copy(maybeBytes->begin(), maybeBytes->end(), reinterpret_cast<std::uint8_t*>(&ipv6.sin6_addr));
                return true;
            }

            break;
        }
        case IPAddressFamily::Invalid:
        default:
            break;
    }

    return false;
}

#endif
