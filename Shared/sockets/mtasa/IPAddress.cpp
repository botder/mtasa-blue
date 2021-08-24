/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPAddress.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "IPAddress.h"
#include <memory>
#include <algorithm>

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

static const std::array<char, 16> hexCharacters = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

namespace mtasa
{
    IPAddress IPAddress::IPv4Any = IPAddress::CreateUnspecified(IPAddressFamily::IPv4);
    IPAddress IPAddress::IPv4Localhost = IPAddress::CreateLocalhost(IPAddressFamily::IPv4);
    IPAddress IPAddress::IPv4Broadcast = IPAddress::CreateBroadcast();

    IPAddress IPAddress::IPv6Any = IPAddress::CreateUnspecified(IPAddressFamily::IPv6);
    IPAddress IPAddress::IPv6Localhost = IPAddress::CreateLocalhost(IPAddressFamily::IPv6);

    IPAddress::IPAddress(const sockaddr_in& address)
    {
        std::copy_n(reinterpret_cast<const std::uint8_t*>(&address.sin_addr), sizeof(address.sin_addr), m_bytes.data());
        m_addressFamily = IPAddressFamily::IPv4;
    }

    IPAddress::IPAddress(const sockaddr_in6& address)
    {
        std::copy_n(reinterpret_cast<const std::uint8_t*>(&address.sin6_addr), sizeof(address.sin6_addr), m_bytes.data());
        m_addressFamily = IPAddressFamily::IPv6;
    }

    IPAddress::IPAddress(const char* numericHostAddress)
    {
        std::vector<IPAddress> addresses = IPAddress::Translate(numericHostAddress, true);

        if (!addresses.empty())
            *this = addresses[0];
    }

    std::string IPAddress::ToString() const
    {
        if (m_addressFamily == IPAddressFamily::IPv4)
        {
            std::array<char, INET_ADDRSTRLEN> buffer{};

            if (inet_ntop(AF_INET, m_bytes.data(), buffer.data(), buffer.size()))
                return std::string{buffer.data()};
        }
        else if (m_addressFamily == IPAddressFamily::IPv6)
        {
            std::array<char, INET6_ADDRSTRLEN> buffer{};

            if (inet_ntop(AF_INET6, m_bytes.data(), buffer.data(), buffer.size()))
                return std::string{buffer.data()};
        }

        return {};
    }

    std::string_view IPAddress::ToString(char* buffer, std::size_t bufferSize) const
    {
        if (m_addressFamily == IPAddressFamily::IPv4)
        {
            if (inet_ntop(AF_INET, m_bytes.data(), buffer, bufferSize))
                return std::string_view{buffer};
        }
        else if (m_addressFamily == IPAddressFamily::IPv6)
        {
            if (inet_ntop(AF_INET6, m_bytes.data(), buffer, bufferSize))
                return std::string_view{buffer};
        }

        return {};
    }

    std::string IPAddress::ToHexString() const
    {
        std::array<char, 32> buffer{};
        std::string_view     view = ToHexString(buffer.data(), buffer.size());
        return {view.data(), view.size()};
    }

    std::string_view IPAddress::ToHexString(char* buffer, std::size_t bufferSize) const
    {
        if (!buffer || !bufferSize)
            return {};

        std::size_t numBytes = 0;

        if (m_addressFamily == IPAddressFamily::IPv4)
            numBytes = 4;
        else if (m_addressFamily == IPAddressFamily::IPv6)
            numBytes = 16;

        if (!numBytes || bufferSize < (numBytes * 2))
            return {};

        for (std::size_t i = 0, j = 0; i < numBytes; i++, j += 2)
        {
            std::uint8_t byte = m_bytes[i];
            buffer[j + 0] = hexCharacters[static_cast<std::size_t>(byte & 0xF0) >> 4];
            buffer[j + 1] = hexCharacters[static_cast<std::size_t>(byte & 0xF)];
        }

        return {buffer, numBytes * 2};
    }

    std::vector<IPAddress> IPAddress::Translate(const char* nodeName, bool isNumericHost)
    {
        std::vector<IPAddress> results;

        if (!nodeName || !nodeName[0])
            return results;

        auto addResult = [&results](int addressFamily, sockaddr* address)
        {
            if (addressFamily == AF_INET)
            {
                auto ipv4 = reinterpret_cast<sockaddr_in*>(address);

                std::array<std::uint8_t, 4> bytes{};
                std::copy_n(reinterpret_cast<std::uint8_t*>(&ipv4->sin_addr), sizeof(ipv4->sin_addr), bytes.data());
                results.emplace_back(bytes);
            }
            else if (addressFamily == AF_INET6)
            {
                auto ipv6 = reinterpret_cast<sockaddr_in6*>(address);

                std::array<std::uint8_t, 16> bytes{};
                std::copy_n(reinterpret_cast<std::uint8_t*>(&ipv6->sin6_addr), sizeof(ipv6->sin6_addr), bytes.data());
                results.emplace_back(bytes);
            }
        };

#ifdef _WIN32
        // On Windows, the function `getaddrinfo` can only translate an ANSI host name to an address.
        // We assume that the provided `nodeName` is a UTF-8 encoded string, and thus we are forced to use
        // the function `GetAddrInfoW`, because we will run into name resolution problems otherwise.
        int wideStringLength = MultiByteToWideChar(CP_UTF8, 0, nodeName, -1, nullptr, 0);

        if (!wideStringLength)
            return results;

        auto wideNodeName = std::make_unique<wchar_t[]>(static_cast<std::size_t>(wideStringLength));

        if (!MultiByteToWideChar(CP_UTF8, 0, nodeName, -1, wideNodeName.get(), wideStringLength))
            return results;

        ADDRINFOW* info = nullptr;

        ADDRINFOW hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = (isNumericHost ? AI_NUMERICHOST : 0);

        if (!GetAddrInfoW(wideNodeName.get(), nullptr, &hints, &info))
        {
            for (ADDRINFOW* i = info; i != nullptr; i = i->ai_next)
                addResult(i->ai_family, i->ai_addr);

            FreeAddrInfoW(info);
        }
#else
        struct addrinfo* info = nullptr;

        struct addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = (isNumericHost ? AI_NUMERICHOST : 0);

        if (!getaddrinfo(nodeName, nullptr, &hints, &info))
        {
            for (addrinfo* i = info; i != nullptr; i = i->ai_next)
                addResult(i->ai_family, i->ai_addr);

            freeaddrinfo(info);
        }
#endif

        return results;
    }

    IPAddress IPAddress::TranslateToIPv4(const char* nodeName, bool isNumericHost)
    {
        for (const IPAddress& address : Translate(nodeName, isNumericHost))
        {
            if (address.IsIPv4())
                return address;
        }

        return {};
    }
}            // namespace mtasa
