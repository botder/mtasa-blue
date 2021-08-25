/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/ASE.cpp
 *  PURPOSE:     Game-Monitor server query protocol handler class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ASE.h"
#include "CLanBroadcast.h"
#include "ASEQuerySDK.h"

using namespace mtasa;

ASE* ASE::_instance = NULL;

ASE::ASE(CMainConfig* pMainConfig, CPlayerManager* pPlayerManager, unsigned short usPort, const SString& strServerIPList /*, bool bLan*/)
    : m_QueryDosProtect(5, 6000, 7000)            // Max of 5 queries per 6 seconds, then 7 second ignore
{
    _instance = this;
    m_tStartTime = time(NULL);

    m_usPortBase = usPort;

    m_pMainConfig = pMainConfig;
    m_pPlayerManager = pPlayerManager;

    m_uiFullLastPlayerCount = 0;
    m_llFullLastTime = 0;
    m_lFullMinInterval = 10 * 1000;            // Update full query cache after 10 seconds

    m_uiLightLastPlayerCount = 0;
    m_llLightLastTime = 0;
    m_lLightMinInterval = 10 * 1000;            // Update light query cache after 10 seconds

    m_uiXfireLightLastPlayerCount = 0;
    m_llXfireLightLastTime = 0;
    m_lXfireLightMinInterval = 10 * 1000;            // Update XFire light query cache after 10 seconds

    m_ulMasterServerQueryCount = 0;
    m_uiNumQueriesTotal = 0;
    m_uiNumQueriesPerMinute = 0;
    m_uiTotalAtMinuteStart = 0;

    m_strGameType = "MTA:SA";
    m_strMapName = "None";
    m_strIPList = strServerIPList;
    m_strPort = std::to_string(usPort);

    m_strMtaAseVersion = MTA_DM_ASE_VERSION;
}

ASE::~ASE()
{
    _instance = NULL;
    ClearRules();
}

bool ASE::SetPortEnabled(bool bInternetEnabled, bool bLanEnabled)
{
    // Calc requirements
    bool   bPortEnableReq = bInternetEnabled || bLanEnabled;
    bool   bLanOnly = !bInternetEnabled && bLanEnabled;
    ushort usPortReq = m_usPortBase + SERVER_LIST_QUERY_PORT_OFFSET;

    // Any change?
    if ((!m_sockets.empty()) == bPortEnableReq && m_usPort == usPortReq)
        return true;

    m_usPort = usPortReq;
    m_sockets.clear();

    if (!bPortEnableReq)
        return true;

    // Start new thingmy
    // If a local IP has been specified, ensure it is used for sending
    std::vector<SString> ipList;
    m_strIPList.Split(",", ipList);

    for (const SString& ip : ipList)
    {
        IPAddress address{IPAddress::IPv4Any};

        if (!ip.empty())
        {
            address = IPAddress{ip.c_str()};

            if (!address.IsIPv4())
                return false;
        }

        IPSocket   socket{address.GetAddressFamily(), IPSocketProtocol::UDP};
        IPEndpoint endpoint{address, m_usPort};

        if (!socket.Create() || !socket.SetAddressReuse(true) || !socket.SetNonBlocking(true) || !socket.Bind(endpoint))
            return false;

        m_sockets.emplace_back(std::move(socket));
    }

    return true;
}

void ASE::DoPulse()
{
    if (m_sockets.empty())
        return;

    m_llCurrentTime = GetTickCount64_();
    m_uiCurrentPlayerCount = m_pPlayerManager->Count();

    IPEndpoint            endpoint;
    std::array<char, 100> buffer{};

    for (IPSocket& socket : m_sockets)
    {
        for (int i = 0; i < 100; i++)
        {
            std::string_view message = socket.ReceiveFrom(endpoint, buffer.data(), buffer.size());

            if (message.empty())
                break;

            m_uiNumQueriesTotal++;

            if (m_QueryDosProtect.GetTotalFloodingCount() < 100)
                if (m_QueryDosProtect.AddConnect(endpoint.GetAddress().ToString()))
                    continue;

            const std::string* response = nullptr;

            switch (message[0])
            {
                case 's':
                {            // ASE protocol query
                    m_ulMasterServerQueryCount++;
                    response = QueryFullCached();
                    break;
                }
                case 'b':
                {            // Our own lighter query for ingame browser
                    response = QueryLightCached();
                    break;
                }
                case 'r':
                {            // Our own lighter query for ingame browser - Release version only
                    response = QueryLightCached();
                    break;
                }
                case 'x':
                {            // Our own lighter query for xfire updates
                    response = QueryXfireLightCached();
                    break;
                }
                case 'v':
                {            // MTA Version (For further possibilities to quick ping, in case we do multiply master servers)
                    response = &m_strMtaAseVersion;
                    break;
                }
            }

            if (response && !response->empty())
            {
                (void)socket.SendTo(endpoint, response->c_str(), response->size());
            }
        }
    }

    // Update stats
    if (m_MinuteTimer.Get() >= 60000)
    {
        m_MinuteTimer.Reset();
        m_uiNumQueriesPerMinute = m_uiNumQueriesTotal - m_uiTotalAtMinuteStart;
        m_uiTotalAtMinuteStart = m_uiNumQueriesTotal;
    }
}

// Protect against a flood of server queries.
// Send cached version unless player count has changed, or last re-cache is older than m_lFullMinInterval
const std::string* ASE::QueryFullCached()
{
    if (m_uiCurrentPlayerCount != m_uiFullLastPlayerCount || m_llCurrentTime - m_llFullLastTime > m_lFullMinInterval || m_strFullCached == "")
    {
        m_strFullCached = QueryFull();
        m_llFullLastTime = m_llCurrentTime;
        m_uiFullLastPlayerCount = m_uiCurrentPlayerCount;
    }
    return &m_strFullCached;
}

std::string ASE::QueryFull()
{
    std::stringstream reply;
    std::stringstream temp;

    reply << "EYE1";
    // game
    reply << (unsigned char)4;
    reply << "mta";
    // port
    reply << (unsigned char)(m_strPort.length() + 1);
    reply << m_strPort;
    // server name
    reply << (unsigned char)(m_pMainConfig->GetServerName().length() + 1);
    reply << m_pMainConfig->GetServerName();
    // game type
    reply << (unsigned char)(m_strGameType.length() + 1);
    reply << m_strGameType;
    // map name
    reply << (unsigned char)(m_strMapName.length() + 1);
    reply << m_strMapName;
    // version
    temp << MTA_DM_ASE_VERSION;
    reply << (unsigned char)(temp.str().length() + 1);
    reply << temp.str();
    // passworded
    reply << (unsigned char)2;
    reply << ((m_pMainConfig->HasPassword()) ? 1 : 0);
    // players count
    temp.str("");
    temp << m_pPlayerManager->CountJoined();
    reply << (unsigned char)(temp.str().length() + 1);
    reply << temp.str();
    // players max
    temp.str("");
    temp << m_pMainConfig->GetMaxPlayers();
    reply << (unsigned char)(temp.str().length() + 1);
    reply << temp.str();

    // rules
    list<CASERule*>::iterator rIter = IterBegin();
    for (; rIter != IterEnd(); rIter++)
    {
        // maybe use a map and std strings for rules?
        reply << (unsigned char)(strlen((*rIter)->GetKey()) + 1);
        reply << (*rIter)->GetKey();
        reply << (unsigned char)(strlen((*rIter)->GetValue()) + 1);
        reply << (*rIter)->GetValue();
    }
    reply << (unsigned char)1;

    // players

    // the flags that tell what data we carry per player ( apparently we need all set cause of GM atm )
    unsigned char ucFlags = 0;
    ucFlags |= 0x01;            // nick
    ucFlags |= 0x02;            // team
    ucFlags |= 0x04;            // skin
    ucFlags |= 0x08;            // score
    ucFlags |= 0x16;            // ping
    ucFlags |= 0x32;            // time

    char     szTemp[256] = {'\0'};
    CPlayer* pPlayer = NULL;

    list<CPlayer*>::const_iterator pIter = m_pPlayerManager->IterBegin();
    for (; pIter != m_pPlayerManager->IterEnd(); pIter++)
    {
        pPlayer = *pIter;
        if (pPlayer->IsJoined())
        {
            reply << ucFlags;
            // nick
            std::string strPlayerName = RemoveColorCodes(pPlayer->GetNick());
            if (strPlayerName.length() == 0)
                strPlayerName = pPlayer->GetNick();
            reply << (unsigned char)(strPlayerName.length() + 1);
            reply << strPlayerName.c_str();
            // team (skip)
            reply << (unsigned char)1;
            // skin (skip)
            reply << (unsigned char)1;
            // score
            const std::string& strScore = pPlayer->GetAnnounceValue("score");
            reply << (unsigned char)(strScore.length() + 1);
            reply << strScore.c_str();
            // ping
            snprintf(szTemp, 255, "%u", pPlayer->GetPing());
            reply << (unsigned char)(strlen(szTemp) + 1);
            reply << szTemp;
            // time (skip)
            reply << (unsigned char)1;
        }
    }

    return reply.str();
}

// Protect against a flood of server queries.
// Send cached version unless player count has changed, or last re-cache is older than m_lLightMinInterval
const std::string* ASE::QueryXfireLightCached()
{
    if (m_uiCurrentPlayerCount != m_uiXfireLightLastPlayerCount || m_llCurrentTime - m_llXfireLightLastTime > m_lXfireLightMinInterval ||
        m_strXfireLightCached == "")
    {
        m_strXfireLightCached = QueryXfireLight();
        m_llXfireLightLastTime = m_llCurrentTime;
        m_uiXfireLightLastPlayerCount = m_uiCurrentPlayerCount;
    }
    return &m_strXfireLightCached;
}

std::string ASE::QueryXfireLight()
{
    std::stringstream reply;

    int     iJoinedPlayers = m_pPlayerManager->CountJoined();
    int     iMaxPlayers = m_pMainConfig->GetMaxPlayers();
    SString strPlayerCount = SString("%d/%d", iJoinedPlayers, iMaxPlayers);

    reply << "EYE3";
    // game
    reply << (unsigned char)4;
    reply << "mta";
    // server name
    reply << (unsigned char)(m_pMainConfig->GetServerName().length() + 1);
    reply << m_pMainConfig->GetServerName();
    // game type
    reply << (unsigned char)(m_strGameType.length() + 1);
    reply << m_strGameType;
    // map name with backwardly compatible large player count
    reply << (unsigned char)(m_strMapName.length() + 1 + strPlayerCount.length() + 1);
    reply << m_strMapName;
    reply << (unsigned char)0;
    reply << strPlayerCount;
    // version
    std::string temp = MTA_DM_ASE_VERSION;
    reply << (unsigned char)(temp.length() + 1);
    reply << temp;
    // passworded
    reply << (unsigned char)((m_pMainConfig->HasPassword()) ? 1 : 0);
    // players count
    reply << (unsigned char)std::min(iJoinedPlayers, 255);
    // players max
    reply << (unsigned char)std::min(iMaxPlayers, 255);

    return reply.str();
}

// Protect against a flood of server queries.
// Send cached version unless player count has changed, or last re-cache is older than m_lLightMinInterval
const std::string* ASE::QueryLightCached()
{
    if (m_uiCurrentPlayerCount != m_uiLightLastPlayerCount || m_llCurrentTime - m_llLightLastTime > m_lLightMinInterval || m_strLightCached == "")
    {
        m_strLightCached = QueryLight();
        m_llLightLastTime = m_llCurrentTime;
        m_uiLightLastPlayerCount = m_uiCurrentPlayerCount;
    }
    return &m_strLightCached;
}

std::string ASE::QueryLight()
{
    std::stringstream reply;

    int              iJoinedPlayers = m_pPlayerManager->CountJoined();
    int              iMaxPlayers = m_pMainConfig->GetMaxPlayers();
    SString          strPlayerCount = SString("%d/%d", iJoinedPlayers, iMaxPlayers);
    SString          strBuildType = SString("%d", MTASA_VERSION_TYPE);
    SString          strBuildNumber = SString("%d", MTASA_VERSION_BUILD);
    SFixedString<32> strPingStatusFixed;
    SFixedString<32> strNetRouteFixed;
    g_pNetServer->GetPingStatus(&strPingStatusFixed);
    g_pNetServer->GetNetRoute(&strNetRouteFixed);
    SString strPingStatus = (const char*)strPingStatusFixed;
    SString strNetRoute = (const char*)strNetRouteFixed;
    SString strUpTime("%d", (uint)(time(NULL) - m_tStartTime));
    SString strHttpPort("%d", m_pMainConfig->GetHTTPPort());

    uint uiExtraDataLength = (strPlayerCount.length() + 1 + strBuildType.length() + 1 + strBuildNumber.length() + 1 + strPingStatus.length() + 1 +
                              strNetRoute.length() + 1 + strUpTime.length() + 1 + strHttpPort.length() + 1);
    uint uiMaxMapNameLength = 250 - uiExtraDataLength;
    m_strMapName = m_strMapName.Left(uiMaxMapNameLength);

    reply << "EYE2";
    // game
    reply << (unsigned char)4;
    reply << "mta";
    // port
    reply << (unsigned char)(m_strPort.length() + 1);
    reply << m_strPort;
    // server name
    reply << (unsigned char)(m_pMainConfig->GetServerName().length() + 1);
    reply << m_pMainConfig->GetServerName();
    // game type
    reply << (unsigned char)(m_strGameType.length() + 1);
    reply << m_strGameType;
    // map name with backwardly compatible large player count, build type and build number
    reply << (unsigned char)(m_strMapName.length() + 1 + uiExtraDataLength);
    reply << m_strMapName;
    reply << (unsigned char)0;
    reply << strPlayerCount;
    reply << (unsigned char)0;
    reply << strBuildType;
    reply << (unsigned char)0;
    reply << strBuildNumber;
    reply << (unsigned char)0;
    reply << strPingStatus;
    reply << (unsigned char)0;
    reply << strNetRoute;
    reply << (unsigned char)0;
    reply << strUpTime;
    reply << (unsigned char)0;
    reply << strHttpPort;
    // version
    std::string temp = MTA_DM_ASE_VERSION;
    reply << (unsigned char)(temp.length() + 1);
    reply << temp;
    // passworded
    reply << (unsigned char)((m_pMainConfig->HasPassword()) ? 1 : 0);
    // serial verification?
    reply << (unsigned char)((m_pMainConfig->GetSerialVerificationEnabled()) ? 1 : 0);
    // players count
    reply << (unsigned char)std::min(iJoinedPlayers, 255);
    // players max
    reply << (unsigned char)std::min(iMaxPlayers, 255);

    // players
    CPlayer* pPlayer = NULL;

    // Keep the packet under 1350 bytes to try to avoid fragmentation
    int iBytesLeft = 1340 - (int)reply.tellp();
    int iPlayersLeft = iJoinedPlayers;

    list<CPlayer*>::const_iterator pIter = m_pPlayerManager->IterBegin();
    for (; pIter != m_pPlayerManager->IterEnd(); pIter++)
    {
        pPlayer = *pIter;
        if (pPlayer->IsJoined())
        {
            // nick
            std::string strPlayerName = RemoveColorCodes(pPlayer->GetNick());
            if (strPlayerName.length() == 0)
                strPlayerName = pPlayer->GetNick();

            // Check if we can fit more names
            iBytesLeft -= strPlayerName.length() + 1;
            if (iBytesLeft < iPlayersLeft--)
                strPlayerName = "";

            reply << (unsigned char)(strPlayerName.length() + 1);
            reply << strPlayerName.c_str();
        }
    }

    return reply.str();
}

CLanBroadcast* ASE::InitLan()
{
    return new CLanBroadcast(m_usPort);
}

void ASE::SetGameType(const char* szGameType)
{
    m_strGameType = SStringX(szGameType).Left(MAX_ASE_GAME_TYPE_LENGTH);
}

void ASE::SetMapName(const char* szMapName)
{
    m_strMapName = SStringX(szMapName).Left(MAX_ASE_MAP_NAME_LENGTH);
}

const char* ASE::GetRuleValue(const char* szKey)
{
    // Limit szKey length
    SString strKeyTemp;
    if (szKey && strlen(szKey) > MAX_RULE_KEY_LENGTH)
    {
        strKeyTemp = SStringX(szKey).Left(MAX_RULE_KEY_LENGTH);
        szKey = *strKeyTemp;
    }

    list<CASERule*>::iterator iter = m_Rules.begin();
    for (; iter != m_Rules.end(); iter++)
    {
        if (strcmp((*iter)->GetKey(), szKey) == 0)
        {
            return (*iter)->GetValue();
        }
    }
    return NULL;
}

void ASE::SetRuleValue(const char* szKey, const char* szValue)
{
    // Limit szKey length
    SString strKeyTemp;
    if (szKey && strlen(szKey) > MAX_RULE_KEY_LENGTH)
    {
        strKeyTemp = SStringX(szKey).Left(MAX_RULE_KEY_LENGTH);
        szKey = *strKeyTemp;
    }

    if (szKey && szKey[0])
    {
        // Limit szValue to 200 characters
        SString strValueTemp;
        if (szValue && strlen(szValue) > MAX_RULE_VALUE_LENGTH)
        {
            strValueTemp = SStringX(szValue).Left(MAX_RULE_VALUE_LENGTH);
            szValue = *strValueTemp;
        }

        list<CASERule*>::iterator iter = m_Rules.begin();
        for (; iter != m_Rules.end(); iter++)
        {
            CASERule* pRule = *iter;
            if (strcmp((*iter)->GetKey(), szKey) == 0)
            {
                if (szValue && szValue[0])
                {
                    (*iter)->SetValue(szValue);
                }
                else
                {
                    // Remove from the list
                    delete pRule;
                    m_Rules.erase(iter);
                }
                // And return
                return;
            }
        }
        m_Rules.push_back(new CASERule(szKey, szValue));
    }
}

bool ASE::RemoveRuleValue(const char* szKey)
{
    // Limit szKey length
    SString strKeyTemp;
    if (szKey && strlen(szKey) > MAX_RULE_KEY_LENGTH)
    {
        strKeyTemp = SStringX(szKey).Left(MAX_RULE_KEY_LENGTH);
        szKey = *strKeyTemp;
    }

    list<CASERule*>::iterator iter = m_Rules.begin();
    for (; iter != m_Rules.end(); iter++)
    {
        CASERule* pRule = *iter;
        if (strcmp(pRule->GetKey(), szKey) == 0)
        {
            delete pRule;
            m_Rules.erase(iter);
            return true;
        }
    }
    return false;
}

void ASE::ClearRules()
{
    list<CASERule*>::iterator iter = m_Rules.begin();
    for (; iter != m_Rules.end(); iter++)
    {
        delete *iter;
    }
    m_Rules.clear();
}
