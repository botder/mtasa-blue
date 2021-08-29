/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPAddress.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "IPAddressFamily.h"
#include "Endianness.h"
#include <vector>
#include <string>

struct sockaddr_in;
struct sockaddr_in6;

namespace mtasa
{
    class IPAddress final
    {
    public:
        static IPAddress IPv4Any;
        static IPAddress IPv4Localhost;
        static IPAddress IPv4Broadcast;

        static IPAddress IPv6Any;
        static IPAddress IPv6Localhost;
        static IPAddress IPv6MulticastAllNodes;

    public:
        // Constructs an invalid address (IPv4 nor IPv6)
        constexpr IPAddress() noexcept = default;

        // Constructs an IPv4 address from four 8-bit integers in network byte order (a.b.c.d)
        explicit constexpr IPAddress(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d) noexcept
            : m_bytes{a, b, c, d}, m_addressFamily{IPAddressFamily::IPv4}
        {
        }

        // Constructs an IPv4 address from an 8-bit integer array in network byte order (bytes[0].bytes[1].bytes[2].bytes[3])
        explicit constexpr IPAddress(const std::array<std::uint8_t, 4>& bytes) noexcept
            : m_bytes{bytes[0], bytes[1], bytes[2], bytes[3]}, m_addressFamily{IPAddressFamily::IPv4}
        {
        }

        // Constructs an IPv4 address from a single 32-bit integer in network byte order (<byte 1>.<byte 2>.<byte 3>.<byte 4>)
        explicit constexpr IPAddress(std::uint32_t a) noexcept
            : m_bytes{static_cast<std::uint8_t>(a & 0xFF), static_cast<std::uint8_t>((a >> 8) & 0xFF), static_cast<std::uint8_t>((a >> 16) & 0xFF),
                      static_cast<std::uint8_t>((a >> 24) & 0xFF)},
              m_addressFamily{IPAddressFamily::IPv4}
        {
        }

        // Constructs an IPv6 address from eight 16-bit integers in network byte order (a:b:c:d:e:f:g:h).
        // Each 16-bit quartet must be coded in host endianness.
        explicit constexpr IPAddress(std::uint16_t a, std::uint16_t b, std::uint16_t c, std::uint16_t d, std::uint16_t e, std::uint16_t f, std::uint16_t g,
                                     std::uint16_t h) noexcept
            : m_bytes{static_cast<std::uint8_t>((a >> 8) & 0xFF), static_cast<std::uint8_t>(a & 0xFF),        static_cast<std::uint8_t>((b >> 8) & 0xFF),
                      static_cast<std::uint8_t>(b & 0xFF),        static_cast<std::uint8_t>((c >> 8) & 0xFF), static_cast<std::uint8_t>(c & 0xFF),
                      static_cast<std::uint8_t>((d >> 8) & 0xFF), static_cast<std::uint8_t>(d & 0xFF),        static_cast<std::uint8_t>((e >> 8) & 0xFF),
                      static_cast<std::uint8_t>(e & 0xFF),        static_cast<std::uint8_t>((f >> 8) & 0xFF), static_cast<std::uint8_t>(f & 0xFF),
                      static_cast<std::uint8_t>((g >> 8) & 0xFF), static_cast<std::uint8_t>(g & 0xFF),        static_cast<std::uint8_t>((h >> 8) & 0xFF),
                      static_cast<std::uint8_t>(h & 0xFF)},
              m_addressFamily{IPAddressFamily::IPv6}
        {
        }

        // Constructs an IPv6 address from an 8-bit integer array in network byte order (bytes[0]bytes[1]:bytes[2]bytes[3]:...)
        explicit constexpr IPAddress(const std::array<std::uint8_t, 16>& bytes)
            : m_bytes{bytes[0], bytes[1], bytes[2],  bytes[3],  bytes[4],  bytes[5],  bytes[6],  bytes[7],
                      bytes[8], bytes[9], bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]},
              m_addressFamily{IPAddressFamily::IPv6}
        {
        }

        // Constructs an IPv4 address from a socket address
        explicit IPAddress(const sockaddr_in& address) noexcept;

        // Constructs an IPv6 address from a socket address
        explicit IPAddress(const sockaddr_in6& address) noexcept;

        // Constructs an IPAddress from a string representation. This supports the following representations:
        // 1) [IPv4] "a.b.c.d" - each part specifies a byte [0-255]
        // 2) [IPv4] "a.b.c" - a and b specify a byte [0-255], c is interpreted as a 16-bit value
        // 3) [IPv4] "a.b" - a specifies a byte [0-255], b is interpreted as a 24-bit value
        // 4) [IPv4] "x" - x is interpreted as a 32-bit value
        // NOTE: In all the above forms, components can be specified in decimal, octal (leading 0) or hexadecimal (leading 0X).
        // 5) [IPv6] "x:x:x:x:x:x:x:x - each x is a 16-bit value with up to 4 hex digits
        // 6) [IPv6] "x:x:x:x:x:x:d.d.d.d" - each x is a 16-bit value with up to 4 hex digits, each d specifies a byte [0-255]
        // NOTE: A series of contiguous zero values, in the IPv6 format, can be abbreviated to "::" and there can be only one instance of "::" in an address.
        explicit IPAddress(const char* numericHostAddress);

    public:
        constexpr IPAddressFamily GetAddressFamily() const noexcept { return m_addressFamily; }

        std::uint32_t GetHostOrderScope() const noexcept { return LoadBigEndian32(m_scope); }

        constexpr std::uint32_t GetNetworkOrderScope() const noexcept { return m_scope; }

        void SetHostOrderScope(std::uint32_t scope) noexcept { m_scope = StoreBigEndian32(scope); }

        void SetNetworkOrderScope(std::uint32_t scope) noexcept { m_scope = scope; }

        const std::uint8_t* GetBytes() const noexcept { return m_bytes; }

    public:
        // Returns true if this is either an IPv4 or IPv6 address
        constexpr bool IsValid() const noexcept { return m_addressFamily != IPAddressFamily::Unspecified; }

        // Returns true if this is neither an IPv4 nor IPv6 address
        constexpr bool IsInvalid() const noexcept { return !IsValid(); }

        // Returns true if this is an IPv4 address
        constexpr bool IsIPv4() const noexcept { return m_addressFamily == IPAddressFamily::IPv4; }

        // Returns true if this is an IPv6 address
        constexpr bool IsIPv6() const noexcept { return m_addressFamily == IPAddressFamily::IPv6; }

        // IPv4: [RFC 1122]
        // IPv6: [RFC 4291]
        // Returns true if this is an unspecified address (0.0.0.0 or ::)
        constexpr bool IsUnspecified() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv4:
                case IPAddressFamily::IPv6:
                    return *this == CreateUnspecified(m_addressFamily);
                case IPAddressFamily::Unspecified:
                default:
                    return false;
            }
        }

        // IPv4: [RFC 1122]
        // IPv6: [RFC 4291]
        // Returns true if this is a loopback address (127.0.0.0/8 or ::1)
        constexpr bool IsLoopback() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv4:
                    return m_bytes[0] == 127;
                case IPAddressFamily::IPv6:
                    return *this == CreateLocalhost(IPAddressFamily::IPv6);
                case IPAddressFamily::Unspecified:
                default:
                    return false;
            }
        }

        // IPv4: [RFC 1918]
        // Returns true if this is an IPv4 private address (10.0.0.0/8 or 172.16.0.0/12 or 192.168.0.0/16)
        constexpr bool IsPrivate() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv4:
                    return m_bytes[0] == 10 || (m_bytes[0] == 172 && (m_bytes[1] & 0xF0) == 16) || (m_bytes[0] == 192 && m_bytes[1] == 168);
                case IPAddressFamily::IPv6:
                case IPAddressFamily::Unspecified:
                default:
                    return false;
            }
        }

        // IPv4: [RFC 6598]
        // Returns true if this is an IPv4 shared address (100.64.0.0/10)
        constexpr bool IsShared() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv4:
                    return m_bytes[0] == 100 && (m_bytes[1] & 0xC0) == 64;
                case IPAddressFamily::IPv6:
                case IPAddressFamily::Unspecified:
                default:
                    return false;
            }
        }

        // IPv4: [RFC 3927]
        // IPv6: [RFC 4291]
        // Returns true if this is a link-local address (169.254.0.0/16 or fe80::/10)
        constexpr bool IsLinkLocal() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv4:
                    return m_bytes[0] == 169 && m_bytes[1] == 254;
                case IPAddressFamily::IPv6:
                    return m_bytes[0] == 0xFE && (m_bytes[1] & 0xC0) == 0x80;
                case IPAddressFamily::Unspecified:
                default:
                    return false;
            }
        }

        // IPv4: [RFC 1112]
        // IPv6: [RFC 2373]
        // Returns true if this is a multicast address (224.0.0.0/4 or ff00::/8)
        constexpr bool IsMulticast() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv4:
                    return (m_bytes[0] & 0xF0) == 224;
                case IPAddressFamily::IPv6:
                    return m_bytes[0] == 0xFF;
                case IPAddressFamily::Unspecified:
                default:
                    return false;
            }
        }

        // IPv4: [RFC 8190][RFC 919]
        // Returns true if this is an IPv4 broadcast address (255.255.255.255)
        constexpr bool IsBroadcast() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv4:
                    return *this == CreateBroadcast();
                case IPAddressFamily::IPv6:
                case IPAddressFamily::Unspecified:
                default:
                    return false;
            }
        }

        // IPv4: [RFC 5771][RFC 6676]
        // IPv6: [RFC 3849]
        // Returns true if this is a documentation address (192.0.2.0/24 or 198.51.100.0/24 or 203.0.113.0/24 or 233.252.0.0/24 or 2001:db8::/32)
        constexpr bool IsDocumentation() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv4:
                    return (m_bytes[0] == 192 && m_bytes[1] == 0 && m_bytes[2] == 2) || (m_bytes[0] == 198 && m_bytes[1] == 51 && m_bytes[2] == 100) ||
                           (m_bytes[0] == 203 && m_bytes[1] == 0 && m_bytes[2] == 113) || (m_bytes[0] == 233 && m_bytes[1] == 252 && m_bytes[2] == 0);
                case IPAddressFamily::IPv6:
                    return m_bytes[0] == 0x20 && m_bytes[1] == 1 && m_bytes[2] == 0x0D && m_bytes[3] == 0xB8;
                case IPAddressFamily::Unspecified:
                default:
                    return false;
            }
        }

        // IPv4: [RFC 2544]
        // IPv6: [RFC 5180][RFC Errata 1752]
        // Returns true if this is a benchmarking address (198.18.0.0/15 or 2001:2::/48)
        constexpr bool IsBenchmarking() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv4:
                    return (m_bytes[0] == 198 && (m_bytes[1] & 0xFE) == 18);
                case IPAddressFamily::IPv6:
                    return (m_bytes[0] == 0x20 && m_bytes[1] == 1 && m_bytes[2] == 0 && m_bytes[3] == 2 && m_bytes[4] == 0);
                case IPAddressFamily::Unspecified:
                default:
                    return false;
            }
        }

        // IPv4: [RFC 6890]
        // IPv6: [RFC 2928]
        // Returns true if this is an IETF protocol assignment address (192.0.0.0/24 or 2001::/23)
        constexpr bool IsIETFProtocolAssignment() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv4:
                    return m_bytes[0] == 192 && m_bytes[1] == 0 && m_bytes[2] == 0;
                case IPAddressFamily::IPv6:
                    return m_bytes[0] == 0x20 && m_bytes[1] == 1 && (m_bytes[2] & 0xFE) == 0;
                case IPAddressFamily::Unspecified:
                default:
                    return false;
            }
        }

        // IPv6: [RFC 6666]
        // Returns true if this is an IPv6 discard-only address (100::/64)
        constexpr bool IsDiscard() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv6:
                    return m_bytes[0] == 1 && m_bytes[1] == 0 && m_bytes[2] == 0 && m_bytes[3] == 0 && m_bytes[4] == 0 && m_bytes[5] == 0 && m_bytes[6] == 0 &&
                           m_bytes[7] == 0;
                case IPAddressFamily::IPv4:
                case IPAddressFamily::Unspecified:
                default:
                    return false;
            }
        }

        // IPv6: [RFC 4291]
        // Returns true if this is an IPv4-mapped IPv6 address (::ffff:0:0/96)
        constexpr bool IsIPv4Mapped() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv6:
                    return m_bytes[0] == 0 && m_bytes[1] == 0 && m_bytes[2] == 0 && m_bytes[3] == 0 && m_bytes[4] == 0 && m_bytes[5] == 0 && m_bytes[6] == 0 &&
                           m_bytes[7] == 0 && m_bytes[8] == 0 && m_bytes[9] == 0 && m_bytes[10] == 0xFF && m_bytes[11] == 0xFF;
                case IPAddressFamily::IPv4:
                case IPAddressFamily::Unspecified:
                default:
                    return false;
            }
        }

    public:
        // Converts an IPv4 address to an IPv4-mapped IPv6 address (a.b.c.d becomes ::ffff:a.b.c.d, anything else becomes invalid)
        constexpr IPAddress ToIPv4Mapped() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv4:
                {
                    return IPAddress{0,
                                     0,
                                     0,
                                     0,
                                     0,
                                     0xFFFF,
                                     static_cast<std::uint16_t>((m_bytes[0] << 8u) | m_bytes[1]),
                                     static_cast<std::uint16_t>((m_bytes[2] << 8u) | m_bytes[3])};
                }
                case IPAddressFamily::IPv6:
                case IPAddressFamily::Unspecified:
                default:
                    return IPAddress{};
            }
        }

        // Converts an IPv4-mapped IPv6 address to an IPv4 address (::ffff:a.b.c.d becomes a.b.c.d, anything else becomes invalid)
        constexpr IPAddress FromIPv4Mapped() const noexcept
        {
            switch (m_addressFamily)
            {
                case IPAddressFamily::IPv6:
                    if (IsIPv4Mapped())
                        return IPAddress{m_bytes[12], m_bytes[13], m_bytes[14], m_bytes[15]};
                    else
                        return IPAddress{};
                case IPAddressFamily::IPv4:
                case IPAddressFamily::Unspecified:
                default:
                    return IPAddress{};
            }
        }

        // Returns a string representation of the ip address ("a.b.c.d" for IPv4, "x:x:x:x:x:x:x:x" for IPv6)
        std::string ToString() const;

        // Fills the buffer with a string representation of the ip address ("a.b.c.d" for IPv4, "x:x:x:x:x:x:x:x" for IPv6)
        // For an IPv4 address the buffer size should be at least 16 and for an IPv6 address it should be at least 46.
        bool ToString(char* buffer, std::size_t bufferSize) const;

        // Returns a hex string representation of the ip address ("AABBCCDD" for IPv4, "AABBCCDDAABBCCDDAABBCCDDAABBCCDD" for IPv6)
        std::string ToHexString() const;

        // Fills the buffer with a hex string representation of the ip address ("AABBCCDD" for IPv4, "AABBCCDDAABBCCDDAABBCCDDAABBCCDD" for IPv6)
        // Returns the total length of the formatted string.
        std::size_t ToHexString(char* buffer, std::size_t bufferSize) const;

    public:
        constexpr int Compare(const IPAddress& other) const noexcept
        {
            if (m_addressFamily != other.m_addressFamily)
                return static_cast<std::int8_t>(m_addressFamily) - static_cast<std::int8_t>(other.m_addressFamily);

            if (m_addressFamily == IPAddressFamily::Unspecified)
                return 0;

            for (std::size_t i = 0; i < 16; i++) // 16 equals std::size(m_bytes)
            {
                if (m_bytes[i] < other.m_bytes[i])
                    return -1;
                if (m_bytes[i] > other.m_bytes[i])
                    return 1;
            }

            if (IsIPv6() && m_scope != other.m_scope)
                return static_cast<std::int32_t>(m_scope) - static_cast<std::int32_t>(other.m_scope);

            return 0;
        }

        constexpr bool operator==(const IPAddress& other) const noexcept { return Compare(other) == 0; }

        constexpr bool operator!=(const IPAddress& other) const noexcept { return Compare(other) != 0; }

        constexpr bool operator<(const IPAddress& other) const noexcept { return Compare(other) < 0; }

        constexpr bool operator>(const IPAddress& other) const noexcept { return Compare(other) > 0; }

        constexpr bool operator<=(const IPAddress& other) const noexcept { return Compare(other) <= 0; }

        constexpr bool operator>=(const IPAddress& other) const noexcept { return Compare(other) >= 0; }

        explicit constexpr operator bool() const noexcept { return !IsInvalid(); }

    public:
        // Creates an ip address that represents localhost (127.0.0.1 for IPv4, ::1 for IPv6)
        static constexpr IPAddress CreateLocalhost(IPAddressFamily addressFamily) noexcept
        {
            switch (addressFamily)
            {
                case IPAddressFamily::IPv4:
                    return IPAddress{127, 0, 0, 1};
                case IPAddressFamily::IPv6:
                    return IPAddress{0, 0, 0, 0, 0, 0, 0, 1};
                case IPAddressFamily::Unspecified:
                default:
                    return IPAddress{};
            }
        }

        // Creates an unspecified ip address (0.0.0.0 for IPv4, :: for IPv6)
        static constexpr IPAddress CreateUnspecified(IPAddressFamily addressFamily) noexcept
        {
            switch (addressFamily)
            {
                case IPAddressFamily::IPv4:
                    return IPAddress{0, 0, 0, 0};
                case IPAddressFamily::IPv6:
                    return IPAddress{0, 0, 0, 0, 0, 0, 0, 0};
                case IPAddressFamily::Unspecified:
                default:
                    return IPAddress{};
            }
        }

        // Creates an IPv4 broadcast ip address
        static constexpr IPAddress CreateBroadcast() noexcept { return IPAddress{255, 255, 255, 255}; }

    public:
        // Translates a node name to a list of IPAddress'es
        //     a) from a numeric representation of an ip address
        //     b) from a resolvable host name
        static std::vector<IPAddress> Translate(const char* nodeName, bool isNumericHost = false);

        // Translates a node name to the first IPv4 address
        static IPAddress TranslateToIPv4(const char* nodeName, bool isNumericHost = false);

    private:
        // An IP address in network byte order (Big-Endian).
        // Large enough to store either an IPv4 or an IPv6 address.
        // For an IPv4 address, the remaining bytes [4..15] must always be zero.
        std::uint8_t m_bytes[16] = {};

        // An IPv6 address scope number in network byte order (Big-Endian).
        // For IPv6 link-local and site-local addresses (read: scoped addresses) we must differentiate
        // the same ip address by the scope id. For link-local addresses (fe80::/10), this specifies the
        // interface number. For site-local addresses (fec0::/10), this specifies a site identifier.
        std::uint32_t m_scope = 0;

        // Address family this IP address belongs to.
        IPAddressFamily m_addressFamily = IPAddressFamily::Unspecified;
    };
}            // namespace mtasa
