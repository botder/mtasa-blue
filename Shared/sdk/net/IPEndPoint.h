/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sdk/net/IPEndPoint.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// The code below requires at least C++17 support
#if __cplusplus >= 201703L

#include "IPAddress.h"
#include <vector>

namespace mtasa
{
    class IPEndPoint final
    {
    public:
        // Constructs an invalid endpoint
        constexpr IPEndPoint() = default;

        explicit IPEndPoint(const IPAddress& address, std::uint16_t port)
            : m_address{ address }
            , m_port{ port }
        {}

        explicit IPEndPoint(IPAddress&& address, std::uint16_t port)
            : m_address{ address }
            , m_port{ port }
        {}

    public:
        constexpr void SetAddress(const IPAddress& address) noexcept { m_address = address; }

        constexpr void SetAddress(IPAddress&& address) noexcept { m_address = address; }

        constexpr void SetPort(std::uint16_t port) noexcept { m_port = port; }

        constexpr void Invalidate() noexcept
        {
            m_address = {};
            m_port = 0;
        }

    public:
        [[nodiscard]] constexpr IPAddressFamily GetAddressFamily() const noexcept { return m_address.GetAddressFamily(); }

        [[nodiscard]] IPAddress&       GetAddress() noexcept { return m_address; }
        [[nodiscard]] const IPAddress& GetAddress() const noexcept { return m_address; }

        [[nodiscard]] std::uint16_t GetPort() const noexcept { return m_port; }

        [[nodiscard]] std::uint16_t GetNetworkByteOrderPort() const noexcept
        {
            return static_cast<std::uint16_t>((m_port >> 8) & 0xFF | (m_port << 8) & 0xFF00);
        }

    public:
        // Returns true if this is neither an IPv4 or IPv6 endpoint (port > 0)
        [[nodiscard]] constexpr bool IsInvalid() const noexcept { return !m_port || m_address.IsInvalid(); }

        // Returns true if this is a valid IPv4 endpoint (port > 0)
        [[nodiscard]] constexpr bool IsIPv4() const noexcept { return m_port && m_address.IsIPv4(); }

        // Returns true if this is a valid IPv6 endpoint (port > 0)
        [[nodiscard]] constexpr bool IsIPv6() const noexcept { return m_port && m_address.IsIPv6(); }

        // Returns true if this is an unspecified endpoint (0.0.0.0 or ::, port > 0)
        [[nodiscard]] constexpr bool IsUnspecified() const noexcept { return m_port && m_address.IsUnspecified(); }

    public:
        [[nodiscard]] constexpr int Compare(const IPEndPoint& other) const noexcept
        {
            if (m_port != other.m_port)
                return static_cast<std::int16_t>(m_port) - static_cast<std::int16_t>(other.m_port);

            return m_address.Compare(other.m_address);
        }

        constexpr bool operator==(const IPEndPoint& other) const noexcept { return Compare(other) == 0; }
        constexpr bool operator!=(const IPEndPoint& other) const noexcept { return Compare(other) != 0; }
        constexpr bool operator<(const IPEndPoint& other) const noexcept { return Compare(other) < 0; }
        constexpr bool operator>(const IPEndPoint& other) const noexcept { return Compare(other) > 0; }
        constexpr bool operator<=(const IPEndPoint& other) const noexcept { return Compare(other) <= 0; }
        constexpr bool operator>=(const IPEndPoint& other) const noexcept { return Compare(other) >= 0; }

        constexpr operator bool() const noexcept { return !IsInvalid(); }

    public:
        bool ToSocketAddress(sockaddr& address, std::size_t& addressSize) const;

    private:
        IPAddress     m_address{};
        std::uint16_t m_port = 0;
    };
}            // namespace mtasa

#endif
