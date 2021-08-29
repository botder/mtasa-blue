/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPEndpoint.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "IPEndpoint.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

static_assert(sizeof(sockaddr_in::sin_addr) == 4, "Invalid size of sockaddr_in::sin_addr");
static_assert(sizeof(sockaddr_in6::sin6_addr) == 16, "Invalid size of sockaddr_in6::sin6_addr");

namespace mtasa
{
    void IPEndpoint::FromSocketAddress(const sockaddr_in& address) noexcept
    {
        m_address = IPAddress{address};
        m_port = LoadBigEndian16(address.sin_port);
    }

    void IPEndpoint::FromSocketAddress(const sockaddr_in6& address) noexcept
    {
        m_address = IPAddress{address};
        m_port = LoadBigEndian16(address.sin6_port);
    }

    bool IPEndpoint::FromSocketAddress(const sockaddr& address) noexcept
    {
        switch (address.sa_family)
        {
            case AF_INET:
            {
                FromSocketAddress(reinterpret_cast<const sockaddr_in&>(address));
                return true;
            }
            case AF_INET6:
            {
                FromSocketAddress(reinterpret_cast<const sockaddr_in6&>(address));
                return true;
            }
            default:
                break;
        }

        return false;
    }

    bool IPEndpoint::ToSocketAddress(sockaddr& address, std::size_t addressSize) const noexcept
    {
        switch (m_address.GetAddressFamily())
        {
            case IPAddressFamily::IPv4:
            {
                if (addressSize >= sizeof(sockaddr_in))
                {
                    auto& ipv4 = reinterpret_cast<sockaddr_in&>(address);
                    ipv4.sin_family = AF_INET;
                    ipv4.sin_port = GetNetworkOrderPort();
                    std::copy_n(m_address.GetBytes(), sizeof(sockaddr_in::sin_addr), reinterpret_cast<std::uint8_t*>(&ipv4.sin_addr));
                    return true;
                }

                break;
            }
            case IPAddressFamily::IPv6:
            {
                if (addressSize >= sizeof(sockaddr_in6))
                {
                    auto& ipv6 = reinterpret_cast<sockaddr_in6&>(address);
                    ipv6.sin6_family = AF_INET6;
                    ipv6.sin6_port = GetNetworkOrderPort();
                    ipv6.sin6_scope_id = m_address.GetNetworkOrderScope();
                    std::copy_n(m_address.GetBytes(), sizeof(sockaddr_in6::sin6_addr), reinterpret_cast<std::uint8_t*>(&ipv6.sin6_addr));
                    return true;
                }

                break;
            }
            case IPAddressFamily::Unspecified:
            {
                std::fill_n(reinterpret_cast<std::uint8_t*>(&address), addressSize, 0);
                return true;
            }
            default:
                break;
        }

        return false;
    }

    std::string IPEndpoint::ToString() const
    {
        IPAddressFamily addressFamily = m_address.GetAddressFamily();
        std::string     address;

        if (addressFamily == IPAddressFamily::IPv4)
            address = m_address.ToString();
        else if (addressFamily == IPAddressFamily::IPv6)
            address = std::string("[") + m_address.ToString() + "]";
        else
            return {};

        if (m_port > 0)
        {
            address += ':';
            address += std::to_string(m_port);
        }

        return address;
    }
}            // namespace mtasa
