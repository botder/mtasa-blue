/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/CQueryReceiver.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <mtasa/IPSocket.h>

struct SQueryInfo
{
    SQueryInfo()
    {
        containingInfo = false;
        port = 0;
        isPassworded = false;
        serials = false;
        players = 0;
        playerSlot = 0;
        buildType = 1;
        buildNum = 0;
        httpPort = 0;
        pingTime = 0;
    }

    bool    containingInfo;
    SString gameName;
    ushort  port;
    SString serverName;
    SString gameType;
    SString mapName;
    SString versionText;
    bool    isPassworded;
    bool    serials;
    ushort  players;
    ushort  playerSlot;
    int     buildType;
    int     buildNum;
    SString netRoute;
    SString upTime;
    ushort  httpPort;
    ushort  pingTime;

    std::vector<SString> playersPool;
};

class CQueryReceiver final
{
public:
    void RequestQuery(const mtasa::IPEndpoint& endpoint);
    void InvalidateSocket();

    SQueryInfo GetServerResponse();

    uint GetElapsedTimeSinceLastQuery() { return static_cast<uint>(m_ElapsedTime.Get()); };

    bool IsSocketValid() const { return m_socket.Exists(); }

private:
    bool ReadString(std::string& strRead, const char* szBuffer, int& i, int nLength);

    mtasa::IPSocket m_socket;
    CElapsedTime    m_ElapsedTime;
};
