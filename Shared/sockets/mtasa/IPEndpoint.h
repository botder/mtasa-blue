/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPEndpoint.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "IPAddress.h"
#include <vector>

struct sockaddr;
struct sockaddr_storage;

namespace mtasa
{
    class IPEndpoint final
    {
    public:
        // Constructs an invalid endpoint
        constexpr IPEndpoint() = default;

        // Constructs an endpoint from an IP address and a port
        explicit IPEndpoint(const IPAddress& address, std::uint16_t port) : m_address{address}, m_port{port} {}

        // Constructs an endpoint from an IP address and a port
        explicit IPEndpoint(IPAddress&& address, std::uint16_t port) : m_address{address}, m_port{port} {}

    public:
        constexpr void SetAddress(const IPAddress& address) noexcept { m_address = address; }

        constexpr void SetAddress(IPAddress&& address) noexcept { m_address = address; }

        constexpr void SetHostOrderPort(std::uint16_t port) noexcept { m_port = port; }

        void SetNetworkOrderPort(std::uint16_t port) noexcept;

        // Fill this endpoint with the IPv4 address information
        void FromSocketAddress(const sockaddr_in& address) noexcept;

        // Fill this endpoint with the IPv6 address information
        void FromSocketAddress(const sockaddr_in6& address) noexcept;

        // Fills this endpoint with the socket address information
        [[nodiscard]] bool FromSocketAddress(const sockaddr& address) noexcept;

        // Fills the socket address structure with the endpoint
        [[nodiscard]] bool ToSocketAddress(sockaddr& address, std::size_t addressSize) const noexcept;

        // Resets this endpoint to an invalid endpoint
        constexpr void Invalidate() noexcept
        {
            m_address = {};
            m_port = 0;
        }

    public:
        [[nodiscard]] constexpr IPAddressFamily GetAddressFamily() const noexcept { return m_address.GetAddressFamily(); }

        [[nodiscard]] IPAddress&       GetAddress() noexcept { return m_address; }
        [[nodiscard]] const IPAddress& GetAddress() const noexcept { return m_address; }

        [[nodiscard]] std::uint16_t GetHostOrderPort() const noexcept { return m_port; }
        [[nodiscard]] std::uint16_t GetNetworkOrderPort() const noexcept;

    public:
        // Returns true if this is neither an IPv4 nor IPv6 endpoint (port > 0)
        [[nodiscard]] constexpr bool IsInvalid() const noexcept { return !m_port || m_address.IsInvalid(); }

        // Returns true if this is a valid IPv4 endpoint (port > 0)
        [[nodiscard]] constexpr bool IsIPv4() const noexcept { return m_port && m_address.IsIPv4(); }

        // Returns true if this is a valid IPv6 endpoint (port > 0)
        [[nodiscard]] constexpr bool IsIPv6() const noexcept { return m_port && m_address.IsIPv6(); }

        // Returns true if this is an unspecified endpoint (0.0.0.0 or ::, port > 0)
        [[nodiscard]] constexpr bool IsUnspecified() const noexcept { return m_port && m_address.IsUnspecified(); }

        // Returns a string representation of the endpoint ("a.b.c.d:port" for IPv4, "[x:x:x:x:x:x:x:x]:port" for IPv6)
        [[nodiscard]] std::string ToString() const;

    public:
        [[nodiscard]] constexpr int Compare(const IPEndpoint& other) const noexcept
        {
            if (m_port != other.m_port)
                return static_cast<std::int16_t>(m_port) - static_cast<std::int16_t>(other.m_port);

            return m_address.Compare(other.m_address);
        }

        constexpr bool operator==(const IPEndpoint& other) const noexcept { return Compare(other) == 0; }
        constexpr bool operator!=(const IPEndpoint& other) const noexcept { return Compare(other) != 0; }
        constexpr bool operator<(const IPEndpoint& other) const noexcept { return Compare(other) < 0; }
        constexpr bool operator>(const IPEndpoint& other) const noexcept { return Compare(other) > 0; }
        constexpr bool operator<=(const IPEndpoint& other) const noexcept { return Compare(other) <= 0; }
        constexpr bool operator>=(const IPEndpoint& other) const noexcept { return Compare(other) >= 0; }

        explicit constexpr operator bool() const noexcept { return !IsInvalid(); }

    private:
        IPAddress     m_address{};
        std::uint16_t m_port = 0;
    };
}            // namespace mtasa
