/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Network.hpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "SharedUtil.Network.h"

#ifdef WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

using namespace SharedUtil;

IPv4Address::IPv4Address(const sockaddr_in* address)
{
    std::memcpy(bytes, &address->sin_addr, 4);
}

void IPv4Address::SetAddress(const sockaddr_in* address)
{
    std::memcpy(bytes, &address->sin_addr, 4);
}

std::string IPv4Address::ToString() const
{
    char buffer[INET_ADDRSTRLEN];

    if (inet_ntop(AF_INET, bytes, buffer, INET_ADDRSTRLEN))
        return std::string(buffer);

    return std::string();
}

IPv6Address::IPv6Address(const sockaddr_in6* address)
{
    std::memcpy(&bytes, &address->sin6_addr, 16);
}

void IPv6Address::SetAddress(const sockaddr_in6* address)
{
    std::memcpy(&bytes, &address->sin6_addr, 16);
}

std::string SharedUtil::IPv6Address::ToString() const
{
    char buffer[INET6_ADDRSTRLEN];

    if (inet_ntop(AF_INET6, bytes, buffer, INET6_ADDRSTRLEN))
        return std::string(buffer);

    return std::string();
}

IPEndPoint::IPEndPoint(const sockaddr* address, std::uint16_t port) : m_address(IPv4Address()), m_port(port)
{
    if (address->sa_family == AF_INET)
    {
        auto ipv4 = reinterpret_cast<const sockaddr_in*>(address);
        m_addressFamily = IPAddressFamily::IPv4;
        m_address = IPv4Address(ipv4);
    }
    else
    {
        auto ipv6 = reinterpret_cast<const sockaddr_in6*>(address);
        m_addressFamily = IPAddressFamily::IPv6;
        m_address = IPv6Address(ipv6);
    }
}
