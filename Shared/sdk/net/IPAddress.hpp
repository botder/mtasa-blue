/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sdk/net/IPAddress.hpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

// The code below requires at least C++17 support
#if __cplusplus >= 201703L

#include "IPAddress.h"
#include <sstream>
#include <iomanip>

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

mtasa::IPAddress::IPAddress(const sockaddr_in* address)
{
    if (address)
    {
        std::copy_n(reinterpret_cast<const std::uint8_t*>(&address->sin_addr), sizeof(address->sin_addr), m_bytes.data());
        m_addressFamily = IPAddressFamily::IPv4;
    }
}

mtasa::IPAddress::IPAddress(const sockaddr_in6* address)
{
    if (address)
    {
        std::copy_n(reinterpret_cast<const std::uint8_t*>(&address->sin6_addr), sizeof(address->sin6_addr), m_bytes.data());
        m_addressFamily = IPAddressFamily::IPv6;
    }
}

mtasa::IPAddress::IPAddress(const char* numericHostAddress)
{
    std::vector<IPAddress> addresses = IPAddress::Translate(numericHostAddress, true);

    if (!addresses.empty())
        *this = addresses[0];
}

std::string mtasa::IPAddress::ToString() const
{
    if (m_addressFamily == IPAddressFamily::IPv4)
    {
        std::array<char, INET_ADDRSTRLEN> buffer{};
        
        if (inet_ntop(AF_INET, m_bytes.data(), buffer.data(), buffer.size()))
            return std::string{ buffer.begin(), buffer.end() };
    }
    else if (m_addressFamily == IPAddressFamily::IPv6)
    {
        std::array<char, INET6_ADDRSTRLEN> buffer{};

        if (inet_ntop(AF_INET6, m_bytes.data(), buffer.data(), buffer.size()))
            return std::string{ buffer.begin(), buffer.end() };
    }

    return {};
}

std::string mtasa::IPAddress::ToHexString() const
{
    std::stringstream ss;
    ss << std::uppercase << std::setfill('0') << std::setw(2);

    if (m_addressFamily == IPAddressFamily::IPv4)
    {
        for (std::size_t i = 0; i < 4; i++)
            ss << m_bytes[i];
    }
    else if (m_addressFamily == IPAddressFamily::IPv6)
    {
        for (std::uint8_t byte : m_bytes)
            ss << byte;
    }

    return ss.str();
}

std::vector<mtasa::IPAddress> mtasa::IPAddress::Translate(const char* nodeName, bool isNumericHost)
{
    std::vector<IPAddress> results;

    if (!nodeName)
        return results;

    struct addrinfo* info = nullptr;

    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    std::fill_n(reinterpret_cast<std::uint8_t*>(&hints), sizeof(hints), static_cast<std::uint8_t>(0));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (isNumericHost)
        hints.ai_flags = AI_NUMERICHOST;

    if (!getaddrinfo(nodeName, nullptr, &hints, &info))
    {
        for (addrinfo* i = info; i != nullptr; i = i->ai_next)
        {
            if (i->ai_family == AF_INET)
            {
                auto ipv4 = reinterpret_cast<sockaddr_in*>(i->ai_addr);

                std::array<std::uint8_t, 4> bytes{};
                std::copy_n(reinterpret_cast<std::uint8_t*>(&ipv4->sin_addr), sizeof(ipv4->sin_addr), bytes.data());
                results.emplace_back(bytes);
            }
            else if (i->ai_family == AF_INET6)
            {
                auto ipv6 = reinterpret_cast<sockaddr_in6*>(i->ai_addr);

                std::array<std::uint8_t, 16> bytes{};
                std::copy_n(reinterpret_cast<std::uint8_t*>(&ipv6->sin6_addr), sizeof(ipv6->sin6_addr), bytes.data());
                results.emplace_back(bytes);
            }
        }

        freeaddrinfo(info);
    }

    return results;
}

mtasa::IPAddress mtasa::IPAddress::TranslateToIPv4(const char* nodeName, bool isNumericHost)
{
    for (const IPAddress& address : Translate(nodeName, isNumericHost))
    {
        if (address.IsIPv4())
            return address;
    }

    return {};
}

#endif
