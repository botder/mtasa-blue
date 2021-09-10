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
    #include <arpa/inet.h>
    #include <netdb.h>
#endif

static_assert(sizeof(sockaddr_in::sin_addr) == 4, "Invalid size of sockaddr_in::sin_addr");
static_assert(sizeof(sockaddr_in6::sin6_addr) == 16, "Invalid size of sockaddr_in6::sin6_addr");

namespace mtasa
{
    bool IPEndpoint::FromString(const char* buffer, std::size_t length, bool allowPortWildcard) noexcept
    {
        if (!buffer || length < 2)            // Shortest IP address is "::"
            return false;

        // Parse the address
        const char* addressFirst = nullptr;
        const char* addressLast = nullptr;

        if (buffer[0] == '[')
        {
            // Find the closing square bracket character
            size_t ipDelimiter = 1;
            while (ipDelimiter < length && buffer[ipDelimiter] != ']')
            {
                ipDelimiter++;
            }

            // Shortest IPv6 address is "[::]" (requires: ipDelimiter >= 3)
            if (ipDelimiter < 3 || ipDelimiter == length)
                return false;

            addressFirst = &buffer[1];
            addressLast = &buffer[ipDelimiter];
            buffer += ipDelimiter + 1;
            length -= ipDelimiter + 1;

            // Move the buffer pointer to the port delimiter, if there is one
            if (length > 0)
            {
                if (length < 2 || buffer[0] != ':')
                    return false;

                buffer++;
                length--;
            }
        }
        else
        {
            // Find the port delimiter character
            size_t portDelimiter = 0;
            while (portDelimiter < length && buffer[portDelimiter] != ':')
            {
                portDelimiter++;
            }

            if (portDelimiter == length || portDelimiter <= 4)            // IPv4 address without port or IPv6 address without port
            {
                addressFirst = &buffer[0];
                addressLast = addressFirst + length;
                length = 0;
            }
            else if (portDelimiter >= 7 && (length - portDelimiter) > 1)            // IPv4 address with port
            {
                addressFirst = &buffer[0];
                addressLast = &buffer[portDelimiter];

                // Move the buffer pointer to the port delimiter
                buffer += portDelimiter + 1;
                length -= portDelimiter + 1;
            }
            else
            {
                return false;
            }
        }

        // Parse the port number
        const char* portFirst = nullptr;
        const char* portLast = nullptr;

        if (length > 0)
        {
            std::size_t portDigits = 1;
            while (portDigits < 5 && portDigits < length && isdigit(buffer[portDigits]))
            {
                portDigits++;
            }

            if (portDigits != length)
                return false;

            portFirst = &buffer[0];
            portLast = &buffer[portDigits];
        }

        // Check if we are going to exceed our buffer capacities
        if ((addressLast - addressFirst) > 128 || (portLast - portFirst) > 5)
            return false;

        // Copy of the address with a null terminator
        std::array<char, 128 + 1> addressBuffer{};
        std::copy(addressFirst, addressLast, addressBuffer.data());

        // Copy of the port number with a null terminator
        std::array<char, 5 + 1> portBuffer{};

        if (portFirst != nullptr)
            std::copy(portFirst, portLast, portBuffer.data());

        // Convert wildcard '*' port to zero (custom logic/special case)
        if (allowPortWildcard && portFirst != nullptr)
        {
            if (*portFirst == '*' && (portLast - portFirst) == 1)
                portBuffer[0] = '0';
        }

        // Translate the input using `getaddrinfo`
        struct addrinfo* info = nullptr;

        struct addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;

        if (!getaddrinfo(addressBuffer.data(), portBuffer.data(), &hints, &info))
        {
            bool success = FromSocketAddress(reinterpret_cast<sockaddr&>(*info->ai_addr));
            freeaddrinfo(info);
            return success;
        }

        return false;
    }

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
                    ipv6.sin6_scope_id = m_address.GetScope();
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

    std::string IPEndpoint::ToString(bool usePortWildcard) const
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
        else if (usePortWildcard)
        {
            address += ":*";
        }

        return address;
    }
}            // namespace mtasa
