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
#include <unordered_set>

using namespace mtasa;

CLanBroadcast::CLanBroadcast(const std::vector<IPBindableEndpoint>& bindings)
{
    bool usingIPv4 = false;
    bool usingIPv6 = false;

    std::unordered_set<std::uint32_t> scopes;

    for (const IPBindableEndpoint& binding : bindings)
    {
        const IPAddress& address = binding.endpoint.GetAddress();

        if (address.IsIPv4())
        {
            usingIPv4 = true;
        }
        else if (address.IsIPv6())
        {
            usingIPv6 = true;
            scopes.insert(address.GetHostOrderScope());

            if (binding.useDualMode)
                usingIPv4 = true;
        }
    }

    if (usingIPv4 && usingIPv6)
    {
        if (!CreateIPv6Socket(true) && !CreateIPv4Socket())
            return;
    }
    else if (usingIPv6)
    {
        if (!CreateIPv6Socket(false))
            return;
    }
    else
    {
        CreateIPv4Socket();
    }

    // Join the multicast group with the address interface indices (scopes)
    if (m_socket.Exists() && m_socket.IsIPv6())
    {
        for (std::uint32_t scope : scopes)
            m_socket.JoinMulticastGroup(IPAddress::IPv6MulticastAllNodes, scope);
    }

    // Set up the query messages
    m_strClientMessage = std::string(SERVER_LIST_CLIENT_BROADCAST_STR) + " " + std::string(MTA_DM_ASE_VERSION);
    m_strServerMessage = std::string(SERVER_LIST_SERVER_BROADCAST_STR) + " " + std::to_string(bindings[0].endpoint.GetHostOrderPort());
}

void CLanBroadcast::DoPulse()
{
    if (!m_socket.Exists())
        return;

    std::array<char, 32> buffer{};
    IPEndpoint           endpoint;
    std::size_t          length = m_socket.ReceiveFrom(endpoint, buffer.data(), buffer.size());
    std::string_view     message{buffer.data(), length};

    if (!message.empty() && message.substr(0, m_strClientMessage.size()) == m_strClientMessage)
    {
        m_socket.SendTo(endpoint, m_strServerMessage.c_str(), m_strServerMessage.size() + 1);
    }
}

bool CLanBroadcast::CreateIPv6Socket(bool useDualMode)
{
    IPSocket socket{IPAddressFamily::IPv6, IPSocketProtocol::UDP};

    if (!socket.Create() || !socket.SetAddressReuse(true) || !socket.SetNonBlocking(true) || !socket.SetIPv6Only(!useDualMode))
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
