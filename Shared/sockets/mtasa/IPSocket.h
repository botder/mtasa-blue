/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPSocket.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "IPEndpoint.h"
#include "IPSocketProtocol.h"
#include "SocketHandle.h"
#include <type_traits>

namespace mtasa
{
    class IPSocket final
    {
    public:
        static constexpr int MAX_LISTEN_BACKLOG = 0x7fffffff;

    public:
        // Constructs an unspecified IP socket.
        // WARNING: You must set the protocol with `SetProtocol` before usage.
        IPSocket() noexcept = default;

        // Constructs an IP socket for a specific protocol
        explicit IPSocket(IPSocketProtocol protocol) noexcept : m_protocol(protocol) {}

        // Constructs an IP socket for an address family and protocol
        explicit IPSocket(IPAddressFamily addressFamily, IPSocketProtocol protocol) noexcept : m_addressFamily{addressFamily}, m_protocol{protocol} {}

        // Constructs an IP socket from an existing socket handle (address family and protocol are not verified)
        explicit IPSocket(SocketHandle handle, IPAddressFamily addressFamily, IPSocketProtocol protocol) noexcept
            : m_addressFamily{addressFamily}, m_protocol{protocol}, m_handle{handle}
        {
        }

        IPSocket(const IPSocket&) noexcept = delete;
        IPSocket& operator=(const IPSocket&) noexcept = delete;

        IPSocket(IPSocket&& other) noexcept { *this = std::move(other); }

        IPSocket& operator=(IPSocket&& other) noexcept
        {
            if (this != &other)
            {
                m_addressFamily = std::exchange(other.m_addressFamily, IPAddressFamily::Unspecified);
                m_protocol = std::exchange(other.m_protocol, IPSocketProtocol::Unspecified);
                m_handle = std::exchange(other.m_handle, INVALID_SOCKET_HANDLE);
            }

            return *this;
        }

        ~IPSocket() noexcept;

    public:
        // Creates the underlying operating system socket
        [[nodiscard]] bool Create() noexcept;

        // Shuts down and closes the underlying socket
        [[nodiscard]] bool Close() noexcept;

        // Connects the underlying socket to an IP endpoint
        // NOTE: For TCP, you can only establish a connection, if the socket is either bound or unbound.
        // NOTE: For UDP, you can set the default endpoint or clear it (with an invalid endpoint).
        [[nodiscard]] bool Connect(const IPEndpoint& endpoint) noexcept;

        // Binds the underlying socket to an IP endpoint (requires an unbound socket)
        [[nodiscard]] bool Bind(const IPEndpoint& endpoint) noexcept;

        // Marks the underlying socket as a passive socket that can be used to accept incoming connection requests (TCP only)
        [[nodiscard]] bool Listen(int backlog = MAX_LISTEN_BACKLOG) noexcept;

        // Receives bytes from the underlying socket without discarding them (requires a connection)
        [[nodiscard]] std::string_view Peek(char* buffer, std::size_t length) noexcept;

        // Receives bytes from the underlying socket without discarding them (UDP bound/connected only)
        [[nodiscard]] std::string_view PeekFrom(IPEndpoint& endpoint, char* buffer, std::size_t length) noexcept;

        // Receives bytes from the underlying socket (requires a connection)
        [[nodiscard]] std::string_view Receive(char* buffer, std::size_t length) noexcept;

        // Receives bytes from the underlying socket (UDP bound/connected only)
        [[nodiscard]] std::string_view ReceiveFrom(IPEndpoint& endpoint, char* buffer, std::size_t length) noexcept;

        // Transfers bytes through the underlying socket (requires a connection)
        [[nodiscard]] std::size_t Send(const char* buffer, std::size_t length) noexcept;

        // Transfers bytes through the underlying socket to the endpoint (UDP only)
        [[nodiscard]] std::size_t SendTo(const IPEndpoint& receiver, const char* buffer, std::size_t length) noexcept;

        // Accepts an incomming connection on the underlying socket (TCP listeners only)
        [[nodiscard]] std::optional<IPSocket> Accept() const noexcept;

    public:
        [[nodiscard]] bool GetOption(int level, int name, char* buffer, unsigned int* length) const;
        [[nodiscard]] bool SetOption(int level, int name, const char* buffer, unsigned int length);
        [[nodiscard]] bool GetBoolean(int level, int name, bool& state) const;
        [[nodiscard]] bool SetBoolean(int level, int name, bool state);
        [[nodiscard]] bool GetErrorCode(int& errorCode) const;
        [[nodiscard]] bool SetKeepAlive(bool keepAlive);
        [[nodiscard]] bool GetKeepAlive(bool& keepAlive) const;
        [[nodiscard]] bool SetBroadcast(bool broadcast);
        [[nodiscard]] bool GetBroadcast(bool& broadcast) const;
        [[nodiscard]] bool SetAddressReuse(bool enabled);
        [[nodiscard]] bool GetAddressReuse(bool& enabled) const;
        [[nodiscard]] bool SetLinger(bool enabled, unsigned short seconds);
        [[nodiscard]] bool GetLinger(bool& enabled, unsigned short& seconds);
        [[nodiscard]] bool GetTransferBufferSize(int& size) const;
        [[nodiscard]] bool SetTransferBufferSize(int size);
        [[nodiscard]] bool GetReceiveBufferSize(int& size) const;
        [[nodiscard]] bool SetReceiveBufferSize(int size);
        [[nodiscard]] bool SetNonBlocking(bool enabled);
        [[nodiscard]] bool SetExclusiveAddressUse(bool enabled);
        [[nodiscard]] bool GetMTU(int& mtu) const;
        [[nodiscard]] bool GetTransferTTL(int& ttl) const;
        [[nodiscard]] bool SetTransferTTL(int ttl);
        [[nodiscard]] bool GetDontFragment(bool& enabled) const;
        [[nodiscard]] bool SetDontFragment(bool enabled);
        [[nodiscard]] bool SetIPv6Only(bool ipv6Only);
        [[nodiscard]] bool GetIPv6Only(bool& ipv6Only) const;
        [[nodiscard]] bool JoinMulticastGroup(const IPAddress& address, std::uint32_t interfaceIndex);
        [[nodiscard]] bool LeaveMulticastGroup(const IPAddress& address, std::uint32_t interfaceIndex);

    public:
        [[nodiscard]] IPEndpoint GetLocalEndpoint() const noexcept;

        [[nodiscard]] IPEndpoint GetPeerEndpoint() const noexcept;

        [[nodiscard]] bool SetAddressFamily(IPAddressFamily addressFamily) noexcept;

        [[nodiscard]] bool SetProtocol(IPSocketProtocol protocol) noexcept;

        [[nodiscard]] IPAddressFamily GetAddressFamily() const noexcept { return m_addressFamily; }

        [[nodiscard]] bool IsIPv4() const noexcept { return m_addressFamily == IPAddressFamily::IPv4; }

        [[nodiscard]] bool IsIPv6() const noexcept { return m_addressFamily == IPAddressFamily::IPv6; }

        [[nodiscard]] IPSocketProtocol GetProtocol() const noexcept { return m_protocol; }

        [[nodiscard]] bool IsTCP() const noexcept { return m_protocol == IPSocketProtocol::TCP; }

        [[nodiscard]] bool IsUDP() const noexcept { return m_protocol == IPSocketProtocol::UDP; }

        [[nodiscard]] bool Exists() const noexcept { return m_handle != INVALID_SOCKET_HANDLE; }

        [[nodiscard]] SocketHandle GetHandle() const noexcept { return m_handle; }

        [[nodiscard]] SocketHandle Release() noexcept { return std::exchange(m_handle, INVALID_SOCKET_HANDLE); }

    private:
        [[nodiscard]] std::string_view Receive(char* buffer, std::size_t length, int flags, IPEndpoint* endpoint) noexcept;

    private:
        IPAddressFamily  m_addressFamily = IPAddressFamily::Unspecified;
        IPSocketProtocol m_protocol = IPSocketProtocol::Unspecified;
        SocketHandle     m_handle = INVALID_SOCKET_HANDLE;
    };
}            // namespace mtasa
