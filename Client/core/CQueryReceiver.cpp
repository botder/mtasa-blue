/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/CQueryReceiver.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <mtasa/SocketAddress.h>

using namespace mtasa;

void CQueryReceiver::RequestQuery(const IPEndpoint& endpoint)
{
    SocketAddress address{};

    if (!endpoint.ToSocketAddress(reinterpret_cast<sockaddr&>(address), sizeof(address)))
        return;

    if (!m_socket.Exists())
    {
        IPSocket socket{endpoint.GetAddressFamily(), IPSocketProtocol::UDP};

        if (!socket.Create() || !socket.SetNonBlocking(true))
            return;

        m_socket = std::move(socket);
    }

    // TODO(botder): Change this code when CNet::SendTo supports IPv6
    // Trailing data to work around 1 byte UDP packet filtering
    if (m_socket.IsIPv4())
        g_pCore->GetNetwork()->SendTo(m_socket.GetHandle(), "r mtasa", 7, 0, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    else
        (void)m_socket.SendTo(endpoint, "r mtasa", 7);

    m_ElapsedTime.Reset();
}

void CQueryReceiver::InvalidateSocket()
{
    (void)m_socket.Close();
}

bool CQueryReceiver::ReadString(std::string& strRead, const char* szBuffer, int& i, int nLength)
{
    if (i <= nLength)
    {
        unsigned char len = szBuffer[i];
        if (i + len <= nLength && len > 0)
        {
            const char* ptr = &szBuffer[i + 1];
            i += len;
            strRead = std::string(ptr, len - 1);
            return true;
        }
        i++;
    }
    return false;
}

SQueryInfo CQueryReceiver::GetServerResponse()
{
    SQueryInfo info;

    if (!m_socket.Exists())
        return info;            // Query not sent

    // Poll the socket
    IPEndpoint                                 endpoint;
    std::array<char, SERVER_LIST_QUERY_BUFFER> buffer{};
    std::string_view                           message = m_socket.ReceiveFrom(endpoint, buffer.data(), buffer.size());

    if (!message.empty())
    {
        // Parse data
        const char* szBuffer = message.data();
        int         len = message.size();

        // Check length
        if (len < 15)
            return info;

        // Check header
        if (strncmp(szBuffer, "EYE2", 4) != 0)
            return info;

        // Calculate the ping/latency
        info.pingTime = m_ElapsedTime.Get();

        // Parse relevant data
        SString strTemp;
        SString strMapTemp;
        int     i = 4;

        // Game
        if (!ReadString(strTemp, szBuffer, i, len))
            return info;
        info.gameName = strTemp;

        // Port (Ignore result as we must already have the correct value)
        if (!ReadString(strTemp, szBuffer, i, len))
            return info;

        // Server name
        if (!ReadString(strTemp, szBuffer, i, len))
            return info;
        info.serverName = strTemp;

        // Game type
        if (!ReadString(strTemp, szBuffer, i, len))
            return info;
        info.gameType = strTemp;

        // Map name
        if (!ReadString(strMapTemp, szBuffer, i, len))
            return info;
        info.mapName = strMapTemp;

        // Version
        if (!ReadString(strTemp, szBuffer, i, len))
            return info;
        info.versionText = strTemp;

        // Got space for password, serial verification, player count, players max?
        if (i + 4 > len)
            return info;

        info.isPassworded = (szBuffer[i++] == 1);
        info.serials = (szBuffer[i++] == 1);
        info.players = (unsigned char)szBuffer[i++];
        info.playerSlot = (unsigned char)szBuffer[i++];

        // Recover large player count if present
        const SString strPlayerCount = strMapTemp.Right(strMapTemp.length() - strlen(strMapTemp) - 1);
        if (!strPlayerCount.empty())
        {
            SString strJoinedPlayers, strMaxPlayers;
            if (strPlayerCount.Split("/", &strJoinedPlayers, &strMaxPlayers))
            {
                info.players = atoi(strJoinedPlayers);
                info.playerSlot = atoi(strMaxPlayers);
            }
        }

        // Recover server build type if present
        const SString strBuildType = strPlayerCount.Right(strPlayerCount.length() - strlen(strPlayerCount) - 1);
        if (!strBuildType.empty())
            info.buildType = atoi(strBuildType);
        else
            info.buildType = 1;

        // Recover server build number if present
        const SString strBuildNumber = strBuildType.Right(strBuildType.length() - strlen(strBuildType) - 1);
        if (!strBuildNumber.empty())
            info.buildNum = atoi(strBuildNumber);
        else
            info.buildNum = 0;

        // Recover server ping status if present
        const SString strPingStatus = strBuildNumber.Right(strBuildNumber.length() - strlen(strBuildNumber) - 1);
        CCore::GetSingleton().GetNetwork()->UpdatePingStatus(*strPingStatus, info.players);

        // Recover server http port if present
        const SString strNetRoute = strPingStatus.Right(strPingStatus.length() - strlen(strPingStatus) - 1);
        const SString strUpTime = strNetRoute.Right(strNetRoute.length() - strlen(strNetRoute) - 1);
        const SString strHttpPort = strUpTime.Right(strUpTime.length() - strlen(strUpTime) - 1);
        if (!strHttpPort.empty())
            info.httpPort = atoi(strHttpPort);

        // Get player nicks
        while (i < len)
        {
            std::string strPlayer;
            try
            {
                if (ReadString(strPlayer, szBuffer, i, len))
                {
                    // Remove color code, unless that results in an empty string
                    SString strResult = RemoveColorCodes(strPlayer.c_str());
                    if (strResult.length() == 0)
                        strResult = strPlayer;
                    info.playersPool.push_back(strResult);
                }
            }
            catch (...)
            {
                // yeah that's what I thought.
                return info;
            }
        }
        InvalidateSocket();
        info.containingInfo = true;
    }

    return info;
}
