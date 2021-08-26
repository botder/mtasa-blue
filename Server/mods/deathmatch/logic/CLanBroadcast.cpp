/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CLanBroadcast.cpp
 *  PURPOSE:     LAN server broadcasting class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLanBroadcast.h"
#include <array>

using namespace mtasa;

CLanBroadcast::CLanBroadcast(unsigned short usServerPort, IPAddressMode addressMode)
{
    // Open the socket on the UDP broadcast port
    if (addressMode == IPAddressMode::IPv4Only)
    {
        if (!CreateIPv4Socket())
            return;
    }
    else
    {
        bool isIPv6Only = (addressMode == IPAddressMode::IPv6Only);

        if (!CreateIPv6Socket(isIPv6Only))
        {
            // Try to create an IPv4 socket, if creating the IPv6 Dual-Stack socket failed
            if (!isIPv6Only || !CreateIPv4Socket())
                return;
        }
    }

    // Set up the query messages
    m_strClientMessage = std::string(SERVER_LIST_CLIENT_BROADCAST_STR) + " " + std::string(MTA_DM_ASE_VERSION);
    m_strServerMessage = std::string(SERVER_LIST_SERVER_BROADCAST_STR) + " " + std::to_string(usServerPort);
}

void CLanBroadcast::DoPulse()
{
    if (!m_socket.Exists())
        return;

    std::array<char, 32> buffer{};
    IPEndpoint           endpoint;
    std::string_view     message = m_socket.ReceiveFrom(endpoint, buffer.data(), buffer.size());

    if (!message.empty() && message.substr(0, m_strClientMessage.size()) == m_strClientMessage)
    {
        (void)m_socket.SendTo(endpoint, m_strServerMessage.c_str(), m_strServerMessage.size() + 1);
    }
}

bool CLanBroadcast::CreateIPv6Socket(bool isIPv6Only)
{
    IPSocket socket{IPAddressFamily::IPv6, IPSocketProtocol::UDP};

    if (!socket.Create() || !socket.SetAddressReuse(true) || !socket.SetNonBlocking(true) || !socket.SetIPv6Only(isIPv6Only))
        return false;

    if (!socket.JoinMulticastGroup(IPAddress::IPv6MulticastAllNodes, 0))
        return false;

    if (!socket.Bind(IPEndpoint{IPAddress::IPv6Any, SERVER_LIST_BROADCAST_PORT}))
        return false;

    m_socket = std::move(socket);
    return true;
}

bool CLanBroadcast::CreateIPv4Socket()
{
    IPSocket socket{IPAddressFamily::IPv4, IPSocketProtocol::UDP};

    if (!socket.Create() || !socket.SetAddressReuse(true) || !socket.SetNonBlocking(true))
        return false;

    if (!socket.Bind(IPEndpoint{IPAddress::IPv4Any, SERVER_LIST_BROADCAST_PORT}))
        return false;

    m_socket = std::move(socket);
    return true;
}
