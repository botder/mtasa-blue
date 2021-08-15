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

bool IPv4Address::SetAddress(const sockaddr_in* address)
{
    if (address)
    {
        std::memcpy(bytes, &address->sin_addr, 4);
        return true;
    }

    return false;
}

bool IPv4Address::SetAddress(const char* address)
{
    if (address)
        return inet_pton(AF_INET, address, bytes) > 0;

    return false;
}

std::string IPv4Address::ToString() const
{
    char buffer[INET_ADDRSTRLEN];

    if (inet_ntop(AF_INET, bytes, buffer, INET_ADDRSTRLEN))
        return std::string(buffer);

    return std::string();
}

constexpr bool IPv4Address::IsPrivate() const noexcept
{
    // 10.0.0.0/8, 127.0.0.0/8, 169.254.0.0/16, 192.168.0.0/16
    return bytes[0] == 10 || bytes[0] == 127 || (bytes[0] == 169 && bytes[1] == 254) || (bytes[0] == 192 && bytes[1] == 168);
}

constexpr bool IPv4Address::IsReserved() const noexcept
{
    // 224.0.0.0/4, 240.0.0.0/4 
    std::uint8_t cidr_4 = bytes[0] & 0xF0;
    
    if (cidr_4 == 224 || cidr_4 == 240)
        return true;

    // 10.0.0.0/8, 127.0.0.0/8
    if (bytes[0] == 10 || bytes[0] == 127)
        return true;

    std::uint8_t cidr_10 = bytes[1] & 0xC0;

    // 100.64.0.0/10
    if (bytes[0] == 100 && cidr_10 == 64)
        return true;

    std::uint8_t cidr_12 = bytes[1] & 0xF0;

    // 172.16.0.0/12
    if (bytes[0] == 172 && cidr_12 == 16)
        return true;

    std::uint8_t cidr_15 = bytes[1] & 0xFE;

    // 198.18.0.0/15
    if (bytes[0] == 198 && cidr_15 == 18)
        return true;

    // 169.254.0.0/16, 192.168.0.0/16
    if ((bytes[0] == 169 && bytes[1] == 254) || (bytes[0] == 192 && bytes[1] == 168))
        return true;

    std::uint32_t cidr_24 = dword & 0xFFFFFF;

    switch (cidr_24)
    {
        case 0x0000C0:  // 192.0.0.0/24
        case 0x0200C0:  // 192.0.2.0/24
        case 0x6358C0:  // 192.88.99.0/24
        case 0x6433C6:  // 198.51.100.0/24
        case 0x7100CB:  // 203.0.113.0/24
        case 0x00FCE9:  // 233.252.0.0/24
            return true;
        default:
            break;
    }

    // 255.255.255.255/32
    return dword == 0xFFFFFFFF;
}

bool IPv6Address::SetAddress(const sockaddr_in6* address)
{
    if (address)
    {
        std::memcpy(&bytes, &address->sin6_addr, 16);
        return true;
    }

    return false;
}

bool IPv6Address::SetAddress(const char* address)
{
    if (address)
        return inet_pton(AF_INET6, address, bytes) > 0;

    return false;
}

std::string IPv6Address::ToString() const
{
    char buffer[INET6_ADDRSTRLEN];

    if (inet_ntop(AF_INET6, bytes, buffer, INET6_ADDRSTRLEN))
        return std::string(buffer);

    return std::string();
}

constexpr bool IPv6Address::IsPrivate() const noexcept
{
    // ::1/128
    if (qwords[0] == 0 && qwords[1] == 0x0100000000000000)
        return true;

    // fc00::/7
    if ((bytes[0] & 0xFE) == 0xFC)
        return true;

    // fe80::/10
    if (bytes[0] == 0xFE && (bytes[1] & 0xC0) == 0x80)
        return true;
    
    return false;
}

constexpr bool IPv6Address::IsReserved() const noexcept
{
    if (IsPrivate())
        return true;

    // ff00::/8
    if (bytes[0] == 0xFF)
        return true;

    // 2002::/16
    if (bytes[0] == 0x20 && bytes[1] == 0x02)
        return true;

    // 2001:db8::/32
    if (dwords[0] == 0xB80D0120)
        return true;

    // 2001:0000::/32
    if (dwords[0] == 0x00000120)
        return true;

    // 2001:20::/28
    if ((dwords[0] & 0xF0FFFFFF) == 0x20000120)
        return true;

    // 100::/64
    if (qwords[0] == 1)
        return true;

    // ::ffff:0:0/96, ::ffff:0:0:0/96
    if (qwords[0] == 0 && (dwords[2] == 0x0000FFFF || dwords[2] == 0xFFFF0000))
        return true;

    // 64:ff9b::/96
    if (qwords[0] == 0x9BFF6400 && dwords[2] == 0)
        return true;

    return false;
}

bool IPAddress::SetAddress(const sockaddr* address)
{
    if (!address)
        return false;

    if (address->sa_family == AF_INET)
    {
        auto ipv4 = reinterpret_cast<const sockaddr_in*>(address);
        m_address.ipv4.SetAddress(ipv4);
        m_addressFamily = IPAddressFamily::IPv4;
        return true;
    }
    else if (address->sa_family == AF_INET6)
    {
        auto ipv6 = reinterpret_cast<const sockaddr_in6*>(address);
        m_address.ipv6.SetAddress(ipv6);
        m_addressFamily = IPAddressFamily::IPv6;
        return true;
    }

    return false;
}

bool IPAddress::SetAddress(const char* address, IPAddressFamily addressFamily)
{
    if (!address)
        return false;

    // Reset the ip address to the `any address` if the string is empty (defaults to IPv4)
    if (!address[0])
    {
        if (addressFamily == IPAddressFamily::IPv6)
        {
            m_address.ipv6.Reset();
            m_addressFamily = IPAddressFamily::IPv6;
        }
        else
        {
            m_address.ipv4.Reset();
            m_addressFamily = IPAddressFamily::IPv4;
        }

        return true;
    }

    std::vector<IPAddress> addresses = IPAddress::Translate(address, addressFamily);

    if (addresses.empty())
        return false;

    if (addressFamily == IPAddressFamily::None)
    {
        // Prefer IPv4 over IPv6 if we are allowed to decide
        for (const IPAddress& ipAddress : addresses)
        {
            if (ipAddress.m_addressFamily == IPAddressFamily::IPv4)
            {
                *this = ipAddress;
                return true;
            }
        }

        // Pick the first address if we didn't find an IPv4 address
        *this = addresses[0];
        return true;
    }

    for (const IPAddress& ipAddress : addresses)
    {
        if (ipAddress.m_addressFamily == addressFamily)
        {
            *this = ipAddress;
            return true;
        }
    }

    return false;
}

std::vector<IPAddress> IPAddress::Translate(const char* nodeName, IPAddressFamily addressFamily)
{
    std::vector<IPAddress> result;

    if (!nodeName)
        return result;

    struct addrinfo* info = nullptr;

    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;

    if (addressFamily == IPAddressFamily::IPv4)
        hints.ai_family = AF_INET;
    else if (addressFamily == IPAddressFamily::IPv6)
        hints.ai_family = AF_INET6;

    if (!getaddrinfo(nodeName, nullptr, &hints, &info))
    {
        for (addrinfo* i = info; i != nullptr; i = i->ai_next)
        {
            if (i->ai_family == AF_INET)
            {
                auto ipv4 = reinterpret_cast<sockaddr_in*>(i->ai_addr);
                result.emplace_back(IPv4Address(ipv4));
            }
            else if (i->ai_family == AF_INET6)
            {
                auto ipv6 = reinterpret_cast<sockaddr_in6*>(i->ai_addr);
                result.emplace_back(IPv6Address(ipv6));
            }
        }

        freeaddrinfo(info);
    }

    return result;
}

std::uint16_t IPEndPoint::GetNetworkByteOrderPort() const noexcept
{
    return htons(m_port);
}

std::vector<IPEndPoint> IPEndPoint::Translate(const char* nodeName, const char* serviceName, IPAddressFamily addressFamily)
{
    std::vector<IPEndPoint> result;

    if (!nodeName && !serviceName)
        return result;

    struct addrinfo* info = nullptr;

    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;

    if (addressFamily == IPAddressFamily::IPv4)
        hints.ai_family = AF_INET;
    else if (addressFamily == IPAddressFamily::IPv6)
        hints.ai_family = AF_INET6;

    if (!getaddrinfo(nodeName, serviceName, &hints, &info))
    {
        for (addrinfo* i = info; i != nullptr; i = i->ai_next)
        {
            if (i->ai_family == AF_INET)
            {
                auto ipv4 = reinterpret_cast<sockaddr_in*>(i->ai_addr);
                result.emplace_back(IPv4Address(ipv4), ntohs(ipv4->sin_port));
            }
            else if (i->ai_family == AF_INET6)
            {
                auto ipv6 = reinterpret_cast<sockaddr_in6*>(i->ai_addr);
                result.emplace_back(IPv6Address(ipv6), ntohs(ipv6->sin6_port));
            }
        }

        freeaddrinfo(info);
    }

    return result;
}
