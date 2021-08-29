/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPSocket.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "IPSocket.h"
#include <utility>
#include <algorithm>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <cerrno>
    #include <poll.h>
#endif

namespace mtasa
{
    static int GetSocketError()
    {
#ifdef _WIN32
        return WSAGetLastError();
#else
        return errno;
#endif
    }

    static void CloseSocket(SocketHandle handle)
    {
#ifdef _WIN32
        shutdown(handle, SD_BOTH);
        closesocket(handle);
#else
        shutdown(handle, SHUT_RDWR);
        close(handle);
#endif
    }

    static int PollSockets(pollfd* sockets, std::size_t length, int timeout)
    {
#ifdef _WIN32
        return WSAPoll(sockets, static_cast<ULONG>(length), timeout);
#else
        return poll(sockets, static_cast<nfds_t>(length), timeout);
#endif
    }

    static int TranslateAddressFamily(IPAddressFamily addressFamily)
    {
        switch (addressFamily)
        {
            case IPAddressFamily::IPv4:
                return AF_INET;
            case IPAddressFamily::IPv6:
                return AF_INET6;
            case IPAddressFamily::Unspecified:
            default:
                break;
        }

        return AF_UNSPEC;
    }

    static std::pair<int, int> TranslateSocketProtocol(IPSocketProtocol protocol)
    {
        switch (protocol)
        {
            case IPSocketProtocol::TCP:
                return {SOCK_STREAM, IPPROTO_TCP};
            case IPSocketProtocol::UDP:
                return {SOCK_DGRAM, IPPROTO_UDP};
            default:
                break;
        }

        return {0, 0};
    }

    IPSocket::~IPSocket() noexcept
    {
        if (Exists())
            CloseSocket(m_handle);
    }

    bool IPSocket::SetAddressFamily(IPAddressFamily addressFamily) noexcept
    {
        if (Exists())
            return false;

        m_addressFamily = addressFamily;
        return true;
    }

    bool IPSocket::SetProtocol(IPSocketProtocol protocol) noexcept
    {
        if (Exists())
            return false;

        m_protocol = protocol;
        return true;
    }

    bool IPSocket::Create() noexcept
    {
        if (Exists())
            return false;

        if (m_addressFamily == IPAddressFamily::Unspecified || m_protocol == IPSocketProtocol::Unspecified)
            return false;

        int domain = TranslateAddressFamily(m_addressFamily);
        std::pair<int, int> protocol = TranslateSocketProtocol(m_protocol);
        m_handle = socket(domain, protocol.first, protocol.second);
        return m_handle != INVALID_SOCKET_HANDLE;
    }

    bool IPSocket::Close() noexcept
    {
        if (!Exists())
            return false;

        CloseSocket(m_handle);
        m_handle = INVALID_SOCKET_HANDLE;
        return true;
    }

    bool IPSocket::Connect(const IPEndpoint& endpoint) noexcept
    {
        if (!Exists())
        {
            IPAddressFamily previousAddressFamily = std::exchange(m_addressFamily, endpoint.GetAddressFamily());

            if (!Create())
            {
                std::exchange(m_addressFamily, previousAddressFamily);
                return false;
            }
        }

        if (endpoint.GetAddressFamily() != m_addressFamily)
            return false;

        sockaddr_storage address{};
        std::size_t      addressSize = sizeof(address);

        if (!endpoint.ToSocketAddress(reinterpret_cast<sockaddr&>(address), addressSize))
            return false;

        return connect(m_handle, reinterpret_cast<sockaddr*>(&address), sizeof(address)) >= 0;
    }

    bool IPSocket::Bind(const IPEndpoint& endpoint) noexcept
    {
        if (!Exists())
        {
            IPAddressFamily previousAddressFamily = std::exchange(m_addressFamily, endpoint.GetAddressFamily());

            if (!Create())
            {
                std::exchange(m_addressFamily, previousAddressFamily);
                return false;
            }
        }

        if (endpoint.GetAddressFamily() != m_addressFamily)
            return false;

        sockaddr_storage address{};
        std::size_t      addressSize = sizeof(address);

        if (!endpoint.ToSocketAddress(reinterpret_cast<sockaddr&>(address), addressSize))
            return false;

        return bind(m_handle, reinterpret_cast<sockaddr*>(&address), sizeof(address)) >= 0;
    }

    bool IPSocket::Listen(int backlog) noexcept
    {
        if (!Exists() || m_protocol != IPSocketProtocol::TCP || backlog < 0)
            return false;

        return listen(m_handle, backlog) >= 0;
    }

    std::size_t IPSocket::Receive(char* buffer, std::size_t length, int flags, IPEndpoint* endpoint) noexcept
    {
        if (!Exists() || !buffer || !length)
            return 0;

        std::intmax_t bytesReceived = 0;

        if (endpoint)
        {
            sockaddr_storage address{};
            socklen_t        addressSize = sizeof(address);

#ifdef _WIN32
            bytesReceived = recvfrom(m_handle, buffer, static_cast<int>(length), flags, reinterpret_cast<sockaddr*>(&address), &addressSize);

            // A truncated message is not an error
            if (bytesReceived < 0 && GetSocketError() == WSAEMSGSIZE)
                bytesReceived = static_cast<std::intmax_t>(length);
#else
            bytesReceived = recvfrom(m_handle, buffer, length, flags, reinterpret_cast<sockaddr*>(&address), &addressSize);
#endif

            if (bytesReceived >= 0)
                (void)endpoint->FromSocketAddress(reinterpret_cast<sockaddr&>(address));
        }
        else
        {
#ifdef _WIN32
            bytesReceived = recv(m_handle, buffer, static_cast<int>(length), flags);
#else
            bytesReceived = recv(m_handle, buffer, length, flags);
#endif
        }

        return (bytesReceived < 0) ? 0 : static_cast<std::size_t>(bytesReceived);
    }

    std::size_t IPSocket::Peek(char* buffer, std::size_t length) noexcept { return Receive(buffer, length, MSG_PEEK, nullptr); }

    std::size_t IPSocket::PeekFrom(IPEndpoint& endpoint, char* buffer, std::size_t length) noexcept
    {
        if (m_protocol != IPSocketProtocol::UDP)
            return 0;

        return Receive(buffer, length, MSG_PEEK, &endpoint);
    }

    [[nodiscard]] std::size_t IPSocket::Receive(char* buffer, std::size_t length) noexcept { return Receive(buffer, length, 0, nullptr); }

    [[nodiscard]] std::size_t IPSocket::ReceiveFrom(IPEndpoint& endpoint, char* buffer, std::size_t length) noexcept
    {
        if (m_protocol != IPSocketProtocol::UDP)
            return 0;

        return Receive(buffer, length, 0, &endpoint);
    }

    std::size_t IPSocket::Send(const char* buffer, std::size_t length) noexcept
    {
        if (!Exists() || !buffer || !length)
            return 0;

#ifdef _WIN32
        std::intmax_t bytesWritten = send(m_handle, buffer, static_cast<int>(length), 0);
#else
        std::intmax_t bytesWritten = send(m_handle, buffer, length, 0);
#endif

        return (bytesWritten < 0) ? 0 : static_cast<std::size_t>(bytesWritten);
    }

    std::size_t IPSocket::SendTo(const IPEndpoint& receiver, const char* buffer, std::size_t length) noexcept
    {
        // TCP is not able to send messages directly to an endpoint
        if (m_protocol == IPSocketProtocol::TCP)
            return Send(buffer, length);

        if (!Exists() || !buffer || !length)
            return 0;

        sockaddr_storage address{};
        std::size_t      addressSize = sizeof(address);

        if (!receiver.ToSocketAddress(reinterpret_cast<sockaddr&>(address), addressSize))
            return 0;

#ifdef _WIN32
        std::intmax_t bytesWritten = sendto(m_handle, buffer, static_cast<int>(length), 0, reinterpret_cast<sockaddr*>(&address), sizeof(address));
#else
        std::intmax_t bytesWritten = sendto(m_handle, buffer, length, 0, reinterpret_cast<sockaddr*>(&address), sizeof(address));
#endif

        return (bytesWritten < 0) ? 0 : static_cast<std::size_t>(bytesWritten);
    }

    bool IPSocket::Accept(IPSocket& newConnection) const noexcept
    {
        if (!Exists() || m_protocol != IPSocketProtocol::TCP)
            return false;

        sockaddr_storage address{};
        socklen_t        addressSize = sizeof(address);
        SocketHandle     handle = accept(m_handle, reinterpret_cast<sockaddr*>(&address), &addressSize);

        if (handle == INVALID_SOCKET_HANDLE)
            return false;

        newConnection = IPSocket{handle, m_addressFamily, m_protocol};
        return true;
    }

    bool IPSocket::IsReadable(int millisecondsTimeout) const noexcept
    {
        if (!Exists())
            return false;

        pollfd sockets[1] = {};
        sockets[0].fd = m_handle;
        sockets[0].events = POLLIN;
        return PollSockets(sockets, 1, millisecondsTimeout) > 0;
    }

    bool IPSocket::IsWritable(int millisecondsTimeout) const noexcept
    {
        if (!Exists())
            return false;

        pollfd sockets[1] = {};
        sockets[0].fd = m_handle;
        sockets[0].events = POLLOUT;
        return PollSockets(sockets, 1, millisecondsTimeout) > 0;
    }

    bool IPSocket::IsErrored(int millisecondsTimeout) const noexcept
    {
        if (!Exists())
            return false;

        pollfd sockets[1] = {};
        sockets[0].fd = m_handle;
        sockets[0].events = POLLIN | POLLOUT;

        if (PollSockets(sockets, 1, millisecondsTimeout) < 1)
            return false;

        return (sockets[0].revents & (POLLERR | POLLHUP | POLLNVAL)) != 0;
    }

    bool IPSocket::GetOption(int level, int name, char* buffer, unsigned int* length) const
    {
        if (!buffer || !length || *length == 0 || !Exists())
            return false;

        return getsockopt(m_handle, level, name, buffer, reinterpret_cast<socklen_t*>(length)) >= 0;
    }

    bool IPSocket::SetOption(int level, int name, const char* buffer, unsigned int length)
    {
        if (!buffer || !length || !Exists())
            return false;

        return setsockopt(m_handle, level, name, buffer, static_cast<socklen_t>(length)) >= 0;
    }

    bool IPSocket::GetBoolean(int level, int name, bool& state) const
    {
        int          value = 0;
        unsigned int length = sizeof(value);

        if (!GetOption(level, name, reinterpret_cast<char*>(&value), &length))
            return false;

        state = value;
        return true;
    }

    bool IPSocket::SetBoolean(int level, int name, bool state)
    {
        int value = state ? 1 : 0;
        return SetOption(level, name, reinterpret_cast<char*>(&value), sizeof(value));
    }

    bool IPSocket::GetErrorCode(int& errorCode) const
    {
        unsigned int length = sizeof(errorCode);
        return GetOption(SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&errorCode), &length);
    }

    bool IPSocket::SetKeepAlive(bool keepAlive) { return SetBoolean(SOL_SOCKET, SO_KEEPALIVE, keepAlive); }

    bool IPSocket::GetKeepAlive(bool& keepAlive) const { return GetBoolean(SOL_SOCKET, SO_KEEPALIVE, keepAlive); }

    bool IPSocket::SetBroadcast(bool broadcast) { return SetBoolean(SOL_SOCKET, SO_BROADCAST, broadcast); }

    bool IPSocket::GetBroadcast(bool& broadcast) const { return GetBoolean(SOL_SOCKET, SO_BROADCAST, broadcast); }

    bool IPSocket::SetAddressReuse(bool enabled) { return SetBoolean(SOL_SOCKET, SO_REUSEADDR, enabled); }

    bool IPSocket::GetAddressReuse(bool& enabled) const { return GetBoolean(SOL_SOCKET, SO_REUSEADDR, enabled); }

    bool IPSocket::SetLinger(bool enabled, unsigned short seconds)
    {
        linger value{};
        value.l_onoff = enabled;
        value.l_linger = seconds;
        return SetOption(SOL_SOCKET, SO_LINGER, reinterpret_cast<const char*>(&value), sizeof(value));
    }

    bool IPSocket::GetLinger(bool& enabled, unsigned short& seconds)
    {
        linger       value{};
        unsigned int length = sizeof(value);

        if (!GetOption(SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&value), &length))
            return false;

        enabled = !!value.l_onoff;
        seconds = value.l_linger;
        return true;
    }

    bool IPSocket::GetTransferBufferSize(int& size) const
    {
        unsigned int length = sizeof(size);
        return GetOption(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&size), &length);
    }

    bool IPSocket::SetTransferBufferSize(int size) { return SetOption(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&size), sizeof(size)); }

    bool IPSocket::GetReceiveBufferSize(int& size) const
    {
        unsigned int length = sizeof(size);
        return GetOption(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&size), &length);
    }

    bool IPSocket::SetReceiveBufferSize(int size) { return SetOption(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&size), sizeof(size)); }

    bool IPSocket::SetNonBlocking(bool enabled)
    {
#ifdef _WIN32
        u_long value = enabled;
        return -1 != ioctlsocket(m_handle, FIONBIO, &value);
#else
        int           flags = fcntl(m_handle, F_GETFL, 0);

        if (flags == -1)
            return false;

        flags = enabled ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
        return -1 != fcntl(m_handle, F_SETFL, flags);
#endif
    }

    bool IPSocket::SetExclusiveAddressUse(bool enabled)
    {
#ifdef _WIN32
        return SetBoolean(SOL_SOCKET, SO_EXCLUSIVEADDRUSE, enabled);
#else
        return false;
#endif
    }

    bool IPSocket::GetMTU(int& mtu) const
    {
#ifdef __APPLE__
        return false;
#else
        unsigned int length = sizeof(mtu);

        if (m_addressFamily == IPAddressFamily::IPv6)
            return GetOption(IPPROTO_IPV6, IPV6_MTU, reinterpret_cast<char*>(&mtu), &length);
        else if (m_addressFamily == IPAddressFamily::IPv4)
            return GetOption(IPPROTO_IP, IP_MTU, reinterpret_cast<char*>(&mtu), &length);
        else
            return false;
#endif
    }

    bool IPSocket::GetTransferTTL(int& ttl) const
    {
        unsigned int length = sizeof(ttl);
        return GetOption(IPPROTO_IP, IP_TTL, reinterpret_cast<char*>(&ttl), &length);
    }

    bool IPSocket::SetTransferTTL(int ttl) { return SetOption(IPPROTO_IP, IP_TTL, reinterpret_cast<const char*>(&ttl), ttl); }

    bool IPSocket::GetDontFragment(bool& enabled) const
    {
#ifdef _WIN32
        return GetBoolean(IPPROTO_IP, IP_DONTFRAGMENT, enabled);
#else
        return false;
#endif
    }

    bool IPSocket::SetDontFragment(bool enabled)
    {
#ifdef _WIN32
        return SetBoolean(IPPROTO_IP, IP_DONTFRAGMENT, enabled);
#else
        return false;
#endif
    }

    bool IPSocket::SetIPv6Only(bool ipv6Only)
    {
        if (m_addressFamily != IPAddressFamily::IPv6)
            return false;

        return SetBoolean(IPPROTO_IPV6, IPV6_V6ONLY, ipv6Only);
    }

    bool IPSocket::GetIPv6Only(bool& ipv6Only) const
    {
        if (m_addressFamily != IPAddressFamily::IPv6)
            return false;

        return GetBoolean(IPPROTO_IPV6, IPV6_V6ONLY, ipv6Only);
    }

    bool IPSocket::JoinMulticastGroup(const IPAddress& address, std::uint32_t interfaceIndex)
    {
        if (m_addressFamily != IPAddressFamily::IPv6 || address.IsIPv4())
            return false;

        ipv6_mreq request{};
        request.ipv6mr_interface = interfaceIndex;
        std::copy_n(address.GetBytes(), sizeof(sockaddr_in6::sin6_addr), reinterpret_cast<std::uint8_t*>(&request.ipv6mr_multiaddr));

        return SetOption(IPPROTO_IPV6, IPV6_JOIN_GROUP, reinterpret_cast<char*>(&request), sizeof(request));
    }

    bool IPSocket::LeaveMulticastGroup(const IPAddress& address, std::uint32_t interfaceIndex)
    {
        if (m_addressFamily != IPAddressFamily::IPv6 || address.IsIPv4())
            return false;

        ipv6_mreq request{};
        request.ipv6mr_interface = interfaceIndex;
        std::copy_n(address.GetBytes(), sizeof(sockaddr_in6::sin6_addr), reinterpret_cast<std::uint8_t*>(&request.ipv6mr_multiaddr));

        return SetOption(IPPROTO_IPV6, IPV6_LEAVE_GROUP, reinterpret_cast<char*>(&request), sizeof(request));
    }

    IPEndpoint IPSocket::GetLocalEndpoint() const noexcept
    {
        if (!Exists())
            return {};

        sockaddr_storage address{};
        socklen_t        length = sizeof(address);

        if (getsockname(m_handle, reinterpret_cast<sockaddr*>(&address), &length) < 0)
            return {};

        IPEndpoint endpoint;
        (void)endpoint.FromSocketAddress(reinterpret_cast<sockaddr&>(address));
        return endpoint;
    }

    IPEndpoint IPSocket::GetPeerEndpoint() const noexcept
    {
        if (!Exists())
            return {};

        sockaddr_storage address{};
        socklen_t        length = sizeof(address);

        if (getpeername(m_handle, reinterpret_cast<sockaddr*>(&address), &length) < 0)
            return {};

        IPEndpoint endpoint;
        (void)endpoint.FromSocketAddress(reinterpret_cast<sockaddr&>(address));
        return endpoint;
    }
}            // namespace mtasa
