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
#include <mtasa/IPEndpoint.h>

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

    void                   SetConnectionType(mtasa::IPAddressFamily connectionType) { m_connectionType = connectionType; }
    mtasa::IPAddressFamily GetConnectionType() const noexcept { return m_connectionType; }
    mtasa::IPAddressFamily GetActiveConnectionType() const noexcept { return m_endpoint.GetAddressFamily(); }

    static void OpenServerFirewall(const mtasa::IPAddress& address, ushort usHttpPort = 80, bool bHighPriority = false);

    static bool StaticProcessPacket(unsigned char ucPacketID, class NetBitStreamInterface& bitStream);

    SString GetJoinSecret();

    std::string    m_strLastHost;
    unsigned short m_usLastPort = 0;
    std::string    m_strLastPassword;

private:
    bool StartConnectionAttempt();

    bool Event_OnCancelClick(CGUIElement* pElement);

    mtasa::IPAddressFamily m_connectionType = mtasa::IPAddressFamily::Unspecified;
    mtasa::IPEndpoint      m_endpoint;

    std::vector<mtasa::IPEndpoint> m_endpoints;

    std::string m_strHost;
    std::string m_strNick;
    std::string m_strPassword;
    bool        m_bIsDetectingVersion = false;
    bool        m_bIsConnecting = false;
    bool        m_bReconnect = false;
    bool        m_bSave = true;
    time_t      m_tConnectStarted = 0;
    size_t      m_connectionAttemptRound = 0;
    size_t      m_connectionAttemptIndex = 0;
    SString     m_strDiscordSecretJoin;

    std::unique_ptr<GUI_CALLBACK> m_onCancelClick;

    std::unique_ptr<CServerListItem> m_serverItem;
    bool                             m_bNotifyServerBrowser = false;

    bool CheckNickProvided(const char* szNick);
};
