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
#include <vector>

struct sockaddr;
struct sockaddr_in;
struct sockaddr_in6;

namespace SharedUtil
{
    enum class IPAddressFamily : std::uint8_t
    {
        None,
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

        constexpr IPv4Address() noexcept : dword(0) {}
        constexpr explicit IPv4Address(std::uint32_t address) noexcept : dword(address) {}
        explicit IPv4Address(const sockaddr_in* address) : IPv4Address() { SetAddress(address); }
        explicit IPv4Address(const char* address) : IPv4Address() { SetAddress(address); }

        bool SetAddress(const sockaddr_in* address);
        bool SetAddress(const char* address);

        constexpr void Reset() noexcept { dword = 0; }

        std::string ToString() const;

        constexpr bool IsAny() const noexcept;
        constexpr bool IsNone() const noexcept;
        constexpr bool IsPrivate() const noexcept;
        constexpr bool IsReserved() const noexcept;

        constexpr std::uint8_t&       operator[](std::size_t index) { return bytes[index]; }
        constexpr const std::uint8_t& operator[](std::size_t index) const { return bytes[index]; }

        constexpr bool operator==(const IPv4Address& other) const noexcept { return dword == other.dword; }
        constexpr bool operator!=(const IPv4Address& other) const noexcept { return dword != other.dword; }
        constexpr bool operator<(const IPv4Address& other) const noexcept { return dword < other.dword; }
        constexpr bool operator>(const IPv4Address& other) const noexcept { return dword > other.dword; }
        constexpr bool operator<=(const IPv4Address& other) const noexcept { return dword <= other.dword; }
        constexpr bool operator>=(const IPv4Address& other) const noexcept { return dword >= other.dword; }
    };

    static constexpr IPv4Address AnyIPv4Address{};
    static constexpr IPv4Address NoneIPv4Address{0xFFFFFFFF};

    constexpr bool IPv4Address::IsAny() const noexcept { return *this == AnyIPv4Address; }
    constexpr bool IPv4Address::IsNone() const noexcept { return *this == NoneIPv4Address; }

    struct IPv6Address
    {
        // Bytes are in network byte order (Big-Endian)
        union
        {
            std::uint8_t  bytes[16];
            std::uint16_t words[8];
            std::uint32_t dwords[4];
            std::uint64_t qwords[2];
        };

        constexpr IPv6Address() noexcept : qwords{0, 0} {}
        constexpr explicit IPv6Address(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) noexcept : dwords{a0, a1, a2, a3} {}
        explicit IPv6Address(const sockaddr_in6* address) : IPv6Address() { SetAddress(address); }
        explicit IPv6Address(const char* address) : IPv6Address() { SetAddress(address); }

        bool SetAddress(const sockaddr_in6* address);
        bool SetAddress(const char* address);

        constexpr void Reset() noexcept
        {
            qwords[0] = 0;
            qwords[1] = 0;
        }

        std::string ToString() const;

        constexpr bool IsAny() const noexcept;
        constexpr bool IsNone() const noexcept;
        constexpr bool IsPrivate() const noexcept;
        constexpr bool IsReserved() const noexcept;

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

    static constexpr IPv6Address AnyIPv6Address{};
    static constexpr IPv6Address NoneIPv6Address{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

    constexpr bool IPv6Address::IsAny() const noexcept { return *this == AnyIPv6Address; }
    constexpr bool IPv6Address::IsNone() const noexcept { return *this == NoneIPv6Address; }

    class IPAddress final
    {
    public:
        constexpr IPAddress() noexcept = default;
        constexpr IPAddress(IPv4Address v4) noexcept : m_address(v4), m_addressFamily(IPAddressFamily::IPv4) {}
        constexpr IPAddress(IPv6Address v6) noexcept : m_address(v6), m_addressFamily(IPAddressFamily::IPv6) {}
        explicit IPAddress(const sockaddr* address) { SetAddress(address); }
        explicit IPAddress(const char* address, IPAddressFamily addressFamily) { SetAddress(address, addressFamily); }

        constexpr IPAddressFamily GetAddressFamily() const noexcept { return m_addressFamily; }

        constexpr IPv4Address&       GetIPv4() noexcept { return m_address.ipv4; }
        constexpr const IPv4Address& GetIPv4() const noexcept { return m_address.ipv4; }

        constexpr IPv6Address&       GetIPv6() noexcept { return m_address.ipv6; }
        constexpr const IPv6Address& GetIPv6() const noexcept { return m_address.ipv6; }

        bool SetAddress(const sockaddr* address);

        bool SetAddress(const char* address, IPAddressFamily addressFamily);

        constexpr void SetAddress(IPv4Address v4) noexcept
        {
            m_address.ipv4 = v4;
            m_addressFamily = IPAddressFamily::IPv4;
        }

        constexpr void SetAddress(IPv6Address v6) noexcept
        {
            m_address.ipv6 = v6;
            m_addressFamily = IPAddressFamily::IPv6;
        }

        void Reset() noexcept { m_addressFamily = IPAddressFamily::None; }

        std::string ToString() const {
            if (m_addressFamily == IPAddressFamily::None)
                return std::string();
            else if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4.ToString();
            else
                return m_address.ipv6.ToString();
        }

        constexpr bool IsAny() const noexcept
        {
            if (m_addressFamily == IPAddressFamily::None)
                return false;
            else if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4.IsAny();
            else
                return m_address.ipv6.IsAny();
        }

        constexpr bool IsNone() const noexcept
        {
            if (m_addressFamily == IPAddressFamily::None)
                return false;
            else if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4.IsNone();
            else
                return m_address.ipv6.IsNone();
        }

        constexpr bool IsPrivate() const noexcept
        {
            if (m_addressFamily == IPAddressFamily::None)
                return false;
            else if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4.IsPrivate();
            else
                return m_address.ipv6.IsPrivate();
        }

        constexpr bool IsReserved() const noexcept
        {
            if (m_addressFamily == IPAddressFamily::None)
                return false;
            else if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4.IsReserved();
            else
                return m_address.ipv6.IsReserved();
        }

    public:
        constexpr std::uint8_t& operator[](std::size_t index)
        {
            if (m_addressFamily == IPAddressFamily::None)
                return m_address.none;
            else if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4[index];
            else
                return m_address.ipv6[index];
        }

        constexpr const std::uint8_t& operator[](std::size_t index) const
        {
            if (m_addressFamily == IPAddressFamily::None)
                return m_address.none;
            else if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4[index];
            else
                return m_address.ipv6[index];
        }

        constexpr operator bool() const noexcept { return m_addressFamily != IPAddressFamily::None; }

        constexpr bool operator==(const IPAddress& other) const noexcept
        {
            if (m_addressFamily == IPAddressFamily::None || m_addressFamily != other.m_addressFamily)
                return false;

            if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4 == other.m_address.ipv4;
            else
                return m_address.ipv6 == other.m_address.ipv6;
        }

        constexpr bool operator!=(const IPAddress& other) const noexcept { return !(*this == other); }

        constexpr bool operator<(const IPAddress& other) const noexcept
        {
            if (m_addressFamily == IPAddressFamily::None || m_addressFamily != other.m_addressFamily)
                return false;

            if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4 < other.m_address.ipv4;
            else
                return m_address.ipv6 < other.m_address.ipv6;
        }

        constexpr bool operator>(const IPAddress& other) const noexcept
        {
            if (m_addressFamily == IPAddressFamily::None || m_addressFamily != other.m_addressFamily)
                return false;

            if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4 > other.m_address.ipv4;
            else
                return m_address.ipv6 > other.m_address.ipv6;
        }

        constexpr bool operator<=(const IPAddress& other) const noexcept
        {
            if (m_addressFamily == IPAddressFamily::None || m_addressFamily != other.m_addressFamily)
                return false;

            if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4 <= other.m_address.ipv4;
            else
                return m_address.ipv6 <= other.m_address.ipv6;
        }

        constexpr bool operator>=(const IPAddress& other) const noexcept
        {
            if (m_addressFamily == IPAddressFamily::None || m_addressFamily != other.m_addressFamily)
                return false;

            if (m_addressFamily == IPAddressFamily::IPv4)
                return m_address.ipv4 >= other.m_address.ipv4;
            else
                return m_address.ipv6 >= other.m_address.ipv6;
        }

    public:
        static std::vector<IPAddress> Translate(const char* nodeName, IPAddressFamily addressFamily);

    private:
        union AddressVariant
        {
            uint8_t     none;
            IPv4Address ipv4;
            IPv6Address ipv6;

            constexpr AddressVariant() noexcept : none(0) {}
            constexpr AddressVariant(IPv4Address v4) noexcept : ipv4(v4) {}
            constexpr AddressVariant(IPv6Address v6) noexcept : ipv6(v6) {}
        };

        AddressVariant  m_address;
        IPAddressFamily m_addressFamily = IPAddressFamily::None;
    };

    class IPEndPoint final
    {
    public:
        constexpr IPEndPoint(IPAddress address, std::uint16_t port) noexcept : m_address(address), m_port(port) {}

        constexpr IPAddressFamily GetAddressFamily() const noexcept { return m_address.GetAddressFamily(); }

        constexpr const IPAddress& GetAddress() const noexcept { return m_address; }

        constexpr void SetAddress(IPAddress address) noexcept { m_address = address; }

        constexpr void SetPort(std::uint16_t port) noexcept { m_port = port; }

        constexpr std::uint16_t GetPort() const noexcept { return m_port; }

        std::uint16_t GetNetworkByteOrderPort() const noexcept;

    public:
        constexpr bool operator==(const IPEndPoint& other) const noexcept { return m_address == other.m_address && m_port == other.m_port; }
        constexpr bool operator!=(const IPEndPoint& other) const noexcept { return m_address != other.m_address || m_port != other.m_port; }
        constexpr bool operator<(const IPEndPoint& other) const noexcept { return m_address < other.m_address && m_port < other.m_port; }
        constexpr bool operator>(const IPEndPoint& other) const noexcept { return m_address > other.m_address && m_port > other.m_port; }
        constexpr bool operator<=(const IPEndPoint& other) const noexcept { return m_address <= other.m_address && m_port <= other.m_port; }
        constexpr bool operator>=(const IPEndPoint& other) const noexcept { return m_address >= other.m_address && m_port >= other.m_port; }

    public:
        static std::vector<IPEndPoint> Translate(const char* nodeName, const char* serviceName, IPAddressFamily addressFamily);

    private:
        IPAddress     m_address;
        std::uint16_t m_port;
    };
}
