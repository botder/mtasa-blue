/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Network.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>

struct sockaddr;
struct sockaddr_in;
struct sockaddr_in6;

namespace SharedUtil
{
    enum class IPAddressFamily : std::uint8_t
    {
        IPv4,
        IPv6,
    };

    struct IPv4Address
    {
        // Bytes are in network byte order (Big-Endian)
        union
        {
            std::uint8_t  bytes[4];
            std::uint32_t dword;
        };

        constexpr IPv4Address() noexcept : dword{0} {}

        explicit IPv4Address(const sockaddr_in* address);

        void SetAddress(const sockaddr_in* address);

        constexpr void Reset() noexcept { dword = 0; }

        std::string ToString() const;

        constexpr std::uint8_t&       operator[](std::size_t index) { return bytes[index]; }
        constexpr const std::uint8_t& operator[](std::size_t index) const { return bytes[index]; }

        constexpr bool operator==(const IPv4Address& other) const noexcept { return dword == other.dword; }
        constexpr bool operator!=(const IPv4Address& other) const noexcept { return dword != other.dword; }
        constexpr bool operator<(const IPv4Address& other) const noexcept { return dword < other.dword; }
        constexpr bool operator>(const IPv4Address& other) const noexcept { return dword > other.dword; }
        constexpr bool operator<=(const IPv4Address& other) const noexcept { return dword <= other.dword; }
        constexpr bool operator>=(const IPv4Address& other) const noexcept { return dword >= other.dword; }
    };

    struct IPv6Address
    {
        // Bytes are in network byte order (Big-Endian)
        union
        {
            std::uint8_t  bytes[16];
            std::uint32_t dwords[4];
            std::uint64_t qwords[2];
        };

        constexpr IPv6Address() noexcept : qwords{0, 0} {}

        explicit IPv6Address(const sockaddr_in6* address);

        void SetAddress(const sockaddr_in6* address);

        constexpr void Reset() noexcept
        {
            qwords[0] = 0;
            qwords[1] = 0;
        }

        std::string ToString() const;

        constexpr std::uint8_t&       operator[](std::size_t index) { return bytes[index]; }
        constexpr const std::uint8_t& operator[](std::size_t index) const { return bytes[index]; }

        constexpr bool operator==(const IPv6Address& other) const noexcept { return qwords[0] == other.qwords[0] && qwords[1] == other.qwords[1]; }
        constexpr bool operator!=(const IPv6Address& other) const noexcept { return qwords[0] != other.qwords[0] || qwords[1] != other.qwords[1]; }

        constexpr bool operator<(const IPv6Address& other) const noexcept
        {
            if (qwords[1] < other.qwords[1])
                return true;
            else if (qwords[1] == other.qwords[1])
                return qwords[0] < other.qwords[0];
            else
                return false;
        }

        constexpr bool operator>(const IPv6Address& other) const noexcept
        {
            if (qwords[1] > other.qwords[1])
                return true;
            else if (qwords[1] == other.qwords[1])
                return qwords[0] > other.qwords[0];
            else
                return false;
        }

        constexpr bool operator<=(const IPv6Address& other) const noexcept
        {
            if (qwords[1] < other.qwords[1])
                return true;
            else if (qwords[1] == other.qwords[1])
                return qwords[0] <= other.qwords[0];
            else
                return false;
        }

        constexpr bool operator>=(const IPv6Address& other) const noexcept
        {
            if (qwords[1] > other.qwords[1])
                return true;
            else if (qwords[1] == other.qwords[1])
                return qwords[0] >= other.qwords[0];
            else
                return false;
        }
    };

    union IPAddress
    {
        IPv4Address ipv4;
        IPv6Address ipv6;

        constexpr IPAddress(IPv4Address v4) noexcept : ipv4{v4} {}
        constexpr IPAddress(IPv6Address v6) noexcept : ipv6{v6} {}
    };

    class IPEndPoint final
    {
    public:
        constexpr IPEndPoint(IPv4Address address, std::uint16_t port) noexcept : m_addressFamily(IPAddressFamily::IPv4), m_address(address), m_port(port) {}
        constexpr IPEndPoint(IPv6Address address, std::uint16_t port) noexcept : m_addressFamily(IPAddressFamily::IPv6), m_address(address), m_port(port) {}

        explicit IPEndPoint(const sockaddr* address, std::uint16_t port);

        constexpr IPAddressFamily GetAddressFamily() const noexcept { return m_addressFamily; }

        constexpr void SetAddress(IPv4Address address) noexcept
        {
            m_address.ipv4 = address;
            m_addressFamily = IPAddressFamily::IPv4;
        }

        constexpr void SetAddress(IPv6Address address) noexcept
        {
            m_address.ipv6 = address;
            m_addressFamily = IPAddressFamily::IPv6;
        }

        constexpr IPAddress GetAddress() const noexcept { return m_address; }

        constexpr void SetPort(std::uint16_t port) noexcept { m_port = port; }

        constexpr std::uint16_t GetPort() const noexcept { return m_port; }

        constexpr std::uint8_t& operator[](std::size_t index)
        {
            return (m_addressFamily == IPAddressFamily::IPv4) ? m_address.ipv4[index] : m_address.ipv6[index];
        }

        constexpr const std::uint8_t& operator[](std::size_t index) const
        {
            return (m_addressFamily == IPAddressFamily::IPv4) ? m_address.ipv4[index] : m_address.ipv6[index];
        }

        constexpr bool operator==(const IPEndPoint& other) const noexcept
        {
            if (m_addressFamily != other.m_addressFamily || m_port != other.m_port)
                return false;

            if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4 == other.m_address.ipv4;
            else
                return m_address.ipv6 == other.m_address.ipv6;
        }

        constexpr bool operator!=(const IPEndPoint& other) const noexcept { return !(*this == other); }

        constexpr bool operator<(const IPEndPoint& other) const noexcept
        {
            if (m_addressFamily != other.m_addressFamily || m_port >= other.m_port)
                return false;

            if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4 < other.m_address.ipv4;
            else
                return m_address.ipv6 < other.m_address.ipv6;
        }

        constexpr bool operator>(const IPEndPoint& other) const noexcept
        {
            if (m_addressFamily != other.m_addressFamily || m_port <= other.m_port)
                return false;

            if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4 > other.m_address.ipv4;
            else
                return m_address.ipv6 > other.m_address.ipv6;
        }

        constexpr bool operator<=(const IPEndPoint& other) const noexcept
        {
            if (m_addressFamily != other.m_addressFamily || m_port > other.m_port)
                return false;

            if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4 <= other.m_address.ipv4;
            else
                return m_address.ipv6 <= other.m_address.ipv6;
        }

        constexpr bool operator>=(const IPEndPoint& other) const noexcept
        {
            if (m_addressFamily != other.m_addressFamily || m_port < other.m_port)
                return false;

            if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4 >= other.m_address.ipv4;
            else
                return m_address.ipv6 >= other.m_address.ipv6;
        }

    private:
        IPAddressFamily m_addressFamily;
        IPAddress       m_address;
        std::uint16_t   m_port;
    };
}
