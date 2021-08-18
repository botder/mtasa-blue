/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CConnectManager.h
 *  PURPOSE:     Header file for connect manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <ctime>
#include <gui/CGUI.h>
#include <ServerBrowser/CServerInfo.h>
#include <net/IPEndPoint.h>

class CConnectManager
{
public:
    CConnectManager();
    ~CConnectManager();

    bool Connect(const char* szHost, unsigned short usPort, const char* szNick, const char* szPassword, bool bNotifyServerBrowser = false, const char* szSecret = nullptr);
    bool Reconnect(const char* szHost, unsigned short usPort, const char* szPassword, bool bSave = true);

    bool Abort();

    void DoPulse();

    void OnServerExists();

    static void OpenServerFirewall(const mtasa::IPAddress& address, ushort usHttpPort = 80, bool bHighPriority = false);

    static bool StaticProcessPacket(unsigned char ucPacketID, class NetBitStreamInterface& bitStream);

    SString GetJoinSecret();

    std::string    m_strLastHost;
    unsigned short m_usLastPort;
    std::string    m_strLastPassword;

private:
    bool Event_OnCancelClick(CGUIElement* pElement);

    mtasa::IPEndPoint m_endPoint;
    std::string       m_strHost;
    std::string       m_strNick;
    std::string       m_strPassword;
    bool              m_bIsDetectingVersion;
    bool              m_bIsConnecting;
    bool              m_bReconnect;
    bool              m_bSave;
    time_t            m_tConnectStarted;
    bool              m_bHasTriedSecondConnect;
    SString           m_strDiscordSecretJoin;

    GUI_CALLBACK* m_pOnCancelClick;

    CServerListItem* m_pServerItem;
    bool             m_bNotifyServerBrowser;

    bool CheckNickProvided(const char* szNick);
};
