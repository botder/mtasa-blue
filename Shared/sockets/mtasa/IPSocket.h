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
        bool Create() noexcept;

        // Shuts down and closes the underlying socket
        bool Close() noexcept;

        // Connects the underlying socket to an IP endpoint
        // NOTE: For TCP, you can only establish a connection, if the socket is either bound or unbound.
        // NOTE: For UDP, you can set the default endpoint or clear it (with an invalid endpoint).
        bool Connect(const IPEndpoint& endpoint) noexcept;

        // Binds the underlying socket to an IP endpoint (requires an unbound socket)
        bool Bind(const IPEndpoint& endpoint) noexcept;

        // Marks the underlying socket as a passive socket that can be used to accept incoming connection requests (TCP only)
        bool Listen(int backlog = MAX_LISTEN_BACKLOG) noexcept;

        // Receives bytes from the underlying socket without discarding them (requires a connection)
        std::size_t Peek(char* buffer, std::size_t length) noexcept;

        // Receives bytes from the underlying socket without discarding them (UDP bound/connected only)
        std::size_t PeekFrom(IPEndpoint& endpoint, char* buffer, std::size_t length) noexcept;

        // Receives bytes from the underlying socket (requires a connection)
        std::size_t Receive(char* buffer, std::size_t length) noexcept;

        // Receives bytes from the underlying socket (UDP bound/connected only)
        std::size_t ReceiveFrom(IPEndpoint& endpoint, char* buffer, std::size_t length) noexcept;

        // Transfers bytes through the underlying socket (requires a connection)
        std::size_t Send(const char* buffer, std::size_t length) noexcept;

        // Transfers bytes through the underlying socket to the endpoint (UDP only)
        std::size_t SendTo(const IPEndpoint& receiver, const char* buffer, std::size_t length) noexcept;

        // Accepts an incoming connection on the underlying socket (TCP listeners only)
        bool Accept(IPSocket& newConnection) const noexcept;

        // Returns true if the socket is readable
        bool IsReadable(int millisecondsTimeout) const noexcept;

        // Returns true if the socket is writable
        bool IsWritable(int millisecondsTimeout) const noexcept;

        // Returns true if the socket has an error, was either disconnected or aborted, or the socket is invalid
        bool IsErrored(int millisecondsTimeout) const noexcept;

    public:
        bool GetOption(int level, int name, char* buffer, unsigned int* length) const;
        bool SetOption(int level, int name, const char* buffer, unsigned int length);
        bool GetBoolean(int level, int name, bool& state) const;
        bool SetBoolean(int level, int name, bool state);
        bool GetErrorCode(int& errorCode) const;
        bool SetKeepAlive(bool keepAlive);
        bool GetKeepAlive(bool& keepAlive) const;
        bool SetBroadcast(bool broadcast);
        bool GetBroadcast(bool& broadcast) const;
        bool SetAddressReuse(bool enabled);
        bool GetAddressReuse(bool& enabled) const;
        bool SetLinger(bool enabled, unsigned short seconds);
        bool GetLinger(bool& enabled, unsigned short& seconds);
        bool GetTransferBufferSize(int& size) const;
        bool SetTransferBufferSize(int size);
        bool GetReceiveBufferSize(int& size) const;
        bool SetReceiveBufferSize(int size);
        bool SetNonBlocking(bool enabled);
        bool SetExclusiveAddressUse(bool enabled);
        bool GetMTU(int& mtu) const;
        bool GetTransferTTL(int& ttl) const;
        bool SetTransferTTL(int ttl);
        bool GetDontFragment(bool& enabled) const;
        bool SetDontFragment(bool enabled);
        bool SetIPv6Only(bool ipv6Only);
        bool GetIPv6Only(bool& ipv6Only) const;
        bool JoinMulticastGroup(const IPAddress& address, std::uint32_t interfaceIndex);
        bool LeaveMulticastGroup(const IPAddress& address, std::uint32_t interfaceIndex);

    public:
        IPEndpoint GetLocalEndpoint() const noexcept;

        IPEndpoint GetPeerEndpoint() const noexcept;

        bool SetAddressFamily(IPAddressFamily addressFamily) noexcept;

        bool SetProtocol(IPSocketProtocol protocol) noexcept;

        IPAddressFamily GetAddressFamily() const noexcept { return m_addressFamily; }

        bool IsIPv4() const noexcept { return m_addressFamily == IPAddressFamily::IPv4; }

        bool IsIPv6() const noexcept { return m_addressFamily == IPAddressFamily::IPv6; }

        IPSocketProtocol GetProtocol() const noexcept { return m_protocol; }

        bool IsTCP() const noexcept { return m_protocol == IPSocketProtocol::TCP; }

        bool IsUDP() const noexcept { return m_protocol == IPSocketProtocol::UDP; }

        bool Exists() const noexcept { return m_handle != INVALID_SOCKET_HANDLE; }

        SocketHandle GetHandle() const noexcept { return m_handle; }

        SocketHandle Release() noexcept { return std::exchange(m_handle, INVALID_SOCKET_HANDLE); }

    private:
        std::size_t Receive(char* buffer, std::size_t length, int flags, IPEndpoint* endpoint) noexcept;

    private:
        IPAddressFamily  m_addressFamily = IPAddressFamily::Unspecified;
        IPSocketProtocol m_protocol = IPSocketProtocol::Unspecified;
        SocketHandle     m_handle = INVALID_SOCKET_HANDLE;
    };
}            // namespace mtasa
