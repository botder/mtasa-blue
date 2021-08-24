/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CServerList.cpp
 *  PURPOSE:     Master server list querying
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CServerBrowser.MasterServerManager.h"
#include "CServerCache.h"

using namespace std;
using namespace mtasa;

extern CCore*              g_pCore;
std::set<CServerListItem*> CServerListItem::ms_ValidServerListItemMap;

CServerList::CServerList()
{
    m_bUpdated = false;
    m_iPass = 0;
    m_strStatus = _("Idle");
    m_nScanned = 0;
    m_nSkipped = 0;
    m_iRevision = 1;
}

CServerList::~CServerList()
{
    Clear();
}

void CServerList::Clear()
{
#if 1
    // Clear all entries
    m_Servers.DeleteAll();
#else
    for (std::list<CServerListItem*>::iterator i = m_Servers.begin(); i != m_Servers.end(); i++)
        (*i)->ResetForRefresh();
#endif
    m_nScanned = 0;
    m_nSkipped = 0;
    m_iRevision++;
    m_llLastTickCount = 0;
}

void CServerList::Pulse()
{
    // Get QueriesPerSecond setting
    int iQueriesPerSecond = 50;
    int iVar;
    CVARS_GET("browser_speed", iVar);
    if (iVar == 0)
        iQueriesPerSecond = 4;
    else if (iVar == 1)
        iQueriesPerSecond = 10;
    else if (iVar == 2)
        iQueriesPerSecond = 50;

    long long llTickCount = GetTickCount64_();
    // Ensure m_llLastTickCount is initialized
    m_llLastTickCount = m_llLastTickCount ? m_llLastTickCount : llTickCount;
    // Ensure m_llLastTickCount is in range
    m_llLastTickCount = Clamp(llTickCount - 1000, m_llLastTickCount, llTickCount);

    // Calc number of queries this pulse
    int iTicksPerQuery = 1000 / std::max(1, iQueriesPerSecond);
    int iDeltaTicks = llTickCount - m_llLastTickCount;
    int iNumQueries = iDeltaTicks / std::max(1, iTicksPerQuery);
    iNumQueries = Clamp(0, iNumQueries, SERVER_LIST_QUERIES_PER_PULSE);
    int iNumTicksUsed = iNumQueries * iTicksPerQuery;
    m_llLastTickCount += iNumTicksUsed;

    unsigned int uiQueriesSent = 0;
    unsigned int uiQueriesResent = 0;
    unsigned int uiRepliesParsed = 0;
    unsigned int uiNoReplies = 0;
    unsigned int uiActiveServers = 0;
    unsigned int uiTotalSlots = 0;
    unsigned int uiOccupiedSlots = 0;
    bool         bRemoveNonResponding = RemoveNonResponding();

    // If a query is going to be done this pass, try to find high priority items first
    if (iNumQueries > 0)
    {
        std::vector<IPEndpoint> endpointList;
        CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetServerBrowser()->GetVisibleEndPointList(endpointList);

        for (std::vector<IPEndpoint>::iterator iter = endpointList.begin(); iter != endpointList.end(); ++iter)
        {
            CServerListItem* pServer = m_Servers.Find(*iter);

            if (pServer && pServer->WaitingToSendQuery())
            {
                std::string strResult = pServer->Pulse((int)(uiQueriesSent /*+ uiQueriesResent*/) < iNumQueries, bRemoveNonResponding);
                assert(strResult == "SentQuery");
                uiQueriesSent++;
                if ((int)uiQueriesSent >= iNumQueries)
                    break;
            }
        }
    }

    // Scan all servers in our list, and keep the value of scanned servers
    for (CServerListIterator i = m_Servers.begin(); i != m_Servers.end(); i++)
    {
        CServerListItem* pServer = *i;
        uint             uiPrevRevision = pServer->uiRevision;
        std::string      strResult = pServer->Pulse((int)(uiQueriesSent /*+ uiQueriesResent*/) < iNumQueries, bRemoveNonResponding);
        if (uiPrevRevision != pServer->uiRevision)
            m_bUpdated |= true;            // Flag GUI update
        if (strResult == "SentQuery")
            uiQueriesSent++;
        else if (strResult == "ResentQuery")
            uiQueriesResent++;
        else if (strResult == "ParsedQuery")
            uiRepliesParsed++;
        else if (strResult == "NoReply")
            uiNoReplies++;

        if (pServer->nMaxPlayers && !pServer->bMaybeOffline && !pServer->MaybeWontRespond())
        {
            uiActiveServers++;
            uiTotalSlots += pServer->nMaxPlayers;
            uiOccupiedSlots += pServer->nPlayers;
        }
    }

    // Check whether we are done scanning
    std::stringstream ss;

    // Store the new number of scanned servers
    m_nScanned += uiRepliesParsed;
    m_nSkipped += uiNoReplies;
    // OutputDebugLine ( SString ( "[Browser] %08x  Size: %d  m_nScanned:%d  m_nSkipped:%d [%d]  iNumQueries: %d", this, m_Servers.size(), m_nScanned,
    // m_nSkipped, m_nScanned+m_nSkipped, iNumQueries ) );

    if (m_nScanned + m_nSkipped == m_Servers.size())
    {
        // We are no longer refreshing
        m_iPass = 0;
    }

    ss << "   ";
    if (uiTotalSlots > 0)
    {
        SString strPlayersString = _tn("player", "players", uiOccupiedSlots);
        ss << uiOccupiedSlots << " " << strPlayersString << " " << _("on") << " ";
    }

    SString strServersString = _tn("server", "servers", uiActiveServers);
    ss << uiActiveServers << " " << strServersString;

    if (m_iPass)
        ss << "...";

    // Update our status message
    m_strStatus = ss.str() + m_strStatus2;
}

// Return true if did add
bool CServerList::AddUnique(const IPEndpoint& endpoint, bool addAtFront)
{
    if (m_Servers.Find(endpoint))
        return false;
    m_Servers.AddUnique(endpoint, addAtFront);
    return true;
}

bool CServerList::Remove(const IPEndpoint& endpoint)
{
    return m_Servers.Remove(endpoint);
}

void CServerList::Refresh()
{            // Assumes we already have a (saved) list of servers, so we just need to refresh

    // Reinitialize each server list item
    for (std::list<CServerListItem*>::iterator iter = m_Servers.begin(); iter != m_Servers.end(); iter++)
    {
        CServerListItem* pOldItem = *iter;
        assert(pOldItem->m_pItemList == &m_Servers);
        pOldItem->ResetForRefresh();
    }

    m_iPass = 1;
    m_nScanned = 0;
    m_nSkipped = 0;
    m_iRevision++;
}

CServerListInternet::CServerListInternet()
{
    m_ElapsedTime.SetMaxIncrement(500);
    m_pMasterServerManager = NewMasterServerManager();
}

CServerListInternet::~CServerListInternet()
{
    delete m_pMasterServerManager;
    m_pMasterServerManager = NULL;
}

void CServerListInternet::Refresh()
{            // Gets the server list from the master server and refreshes
    m_ElapsedTime.Reset();
    m_pMasterServerManager->Refresh();
    m_iPass = 1;
    m_bUpdated = true;

    // Clear the previous server list
    Clear();
}

static bool SortByASEVersionCallback(const CServerListItem* const d1, const CServerListItem* const d2)
{
    return d1->strVersionSortKey < d2->strVersionSortKey;
};

//
// Ensure serverlist is sorted by MTA ASE version
//
void CServerList::SortByASEVersion()
{
    m_Servers.GetList().sort(SortByASEVersionCallback);
}

void CServerListInternet::Pulse()
{            // We also need to take care of the master server list here
    unsigned long ulTime = m_ElapsedTime.Get();

    if (m_iPass == 1)
    {
        // We are polling for the master server list (first pass)
        m_strStatus = SString(_("Requesting master server list (%lu ms elapsed)"), ulTime);
        m_strStatus2 = "";
        m_bUpdated = true;

        // Attempt to get the data
        if (m_pMasterServerManager->HasData())
        {
            if (m_pMasterServerManager->ParseList(m_Servers))
            {
                m_iPass++;
                GetServerCache()->GetServerListCachedInfo(this);
                SortByASEVersion();
            }
            else
            {
                // Abort
                m_strStatus = _("Master server list could not be parsed.");
                m_iPass = 0;
            }
        }
        else
        {
            // Take care of timeouts
            if (ulTime > SERVER_LIST_MASTER_TIMEOUT)
            {
                // Abort
                m_strStatus = _("Master server list could not be retrieved.");
                m_iPass = 0;
            }
        }
        if (m_iPass == 0)
        {
            // If query failed, load from backup
            GetServerCache()->GenerateServerList(this);
            GetServerCache()->GetServerListCachedInfo(this);
            SortByASEVersion();
            m_strStatus2 = "  " + _("(Backup server list)");
            m_iPass = 2;
        }
    }
    else if (m_iPass == 2)
    {
        // We are scanning our known servers (second pass)
        CServerList::Pulse();
    }
}

void CServerListLAN::Pulse()
{
    if (!m_socket.Exists())
        return;

    // Broadcast the discover packet on a regular interval
    if ((CClientTime::GetTime() - m_ulStartTime) > SERVER_LIST_BROADCAST_REFRESH)
        Discover();

    // Poll our socket to discover any new servers
    IPEndpoint           endpoint;
    std::array<char, 32> buffer{};
    std::string_view     message = m_socket.ReceiveFrom(endpoint, buffer.data(), buffer.size());

    if (message.size() > sizeof(SERVER_LIST_SERVER_BROADCAST_STR))
    {
        if (message.substr(0, sizeof(SERVER_LIST_SERVER_BROADCAST_STR) - 1) == SERVER_LIST_SERVER_BROADCAST_STR)
        {
            auto port = static_cast<unsigned short>(strtol(message.data() + sizeof(SERVER_LIST_SERVER_BROADCAST_STR), nullptr, 10));
            AddUnique(IPEndpoint(endpoint.GetAddress(), port - SERVER_LIST_QUERY_PORT_OFFSET));
        }
    }

    // Scan our already known servers
    CServerList::Pulse();
}

// Gets the server list from LAN-broadcasting servers
void CServerListLAN::Refresh()
{
    m_iPass = 1;
    m_bUpdated = true;
    (void)m_socket.Close();

    // Create the LAN-broadcast socket
    IPSocket socket{IPAddressFamily::IPv4, IPSocketProtocol::UDP};

    if (!socket.Create() || !socket.SetAddressReuse(true) || !socket.SetBroadcast(true) || !socket.SetNonBlocking(true))
    {
        m_strStatus = _("Cannot bind LAN-broadcast socket");
        return;
    }

    m_socket = std::move(socket);

    // Clear the previous server list
    Clear();

    // Discover other servers by sending out the broadcast packet
    Discover();
}

void CServerListLAN::Discover()
{
    m_strStatus = _("Attempting to discover LAN servers");

    // Send out the broadcast packet
    IPEndpoint  endpoint{IPAddress::IPv4Broadcast, SERVER_LIST_BROADCAST_PORT};
    std::string query = std::string(SERVER_LIST_CLIENT_BROADCAST_STR) + " " + std::string(MTA_DM_ASE_VERSION);
    (void)m_socket.SendTo(endpoint, query.c_str(), query.size() + 1);

    // Keep the time
    m_ulStartTime = CClientTime::GetTime();
}

std::string CServerListItem::Pulse(bool bCanSendQuery, bool bRemoveNonResponding)
{            // Queries the server on it's query port (ASE protocol)
    // and returns whether it is done scanning
    if (bScanned || bSkipped)
        return "Done";

    char szBuffer[SERVER_LIST_QUERY_BUFFER] = {0};

    if (WaitingToSendQuery())
    {
        if (!bCanSendQuery)
            return "NotSentQuery";
        Query();
        return "SentQuery";
    }
    else
    {
        // Poll the socket and parse the packet
        if (ParseQuery())
        {
            bMaybeOffline = false;
            SetDataQuality(SERVER_INFO_QUERY);
            uiCacheNoReplyCount = 0;
            uiRevision++;                                           // To flag browser gui update
            GetServerCache()->SetServerCachedInfo(this);            // Save parsed info in the cache
            return "ParsedQuery";
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        // TCP send to servers that the master server can see, but this client can't
        uint uiQueryAge = queryReceiver.GetElapsedTimeSinceLastQuery();
        if (uiQueryAge > 2000)
        {
            if (!m_bDoneTcpSend && GetMaxRetries() > 0)
            {
                m_bDoneTcpSend = true;
                if ((m_ucSpecialFlags & ASE_SPECIAL_FLAG_DENY_TCP_SEND) == 0)
                {
                    CConnectManager::OpenServerFirewall(endpoint.GetAddress(), m_usHttpPort, false);
                    m_bDoPostTcpQuery = 1;
                }
            }
        }
        if (uiQueryAge > 4000)
        {
            if (m_bDoPostTcpQuery)
            {
                // Do another query if we just tried to open this server's firewall
                m_bDoPostTcpQuery = false;
                Query();
                return "ResentQuery";
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////

        if (uiQueryAge > SERVER_LIST_ITEM_TIMEOUT)
        {
            if (bKeepFlag)
                bRemoveNonResponding = false;

            if (bRemoveNonResponding)
            {
                bMaybeOffline = true;            // Flag to help 'Include offline' browser option
                nPlayers = 0;                    // We don't have player names, so zero this now
            }
            uiRevision++;            // To flag browser gui update

            if (uiQueryRetryCount < GetMaxRetries())
            {
                // Try again
                uiQueryRetryCount++;
                uiRevision++;            // To flag browser gui update
                if (GetDataQuality() > SERVER_INFO_ASE_0)
                    GetServerCache()->SetServerCachedInfo(this);
                Query();
                return "ResentQuery";
            }
            else
            {
                // Give up
                queryReceiver.InvalidateSocket();
                uiRevision++;            // To flag browser gui update

                if (bRemoveNonResponding)
                {
                    uiCacheNoReplyCount++;            // Keep a persistent count of failures. (When uiCacheNoReplyCount gets to 3, the server is removed from
                                                      // the Server Cache)
                    bSkipped = true;
                    if (GetDataQuality() > SERVER_INFO_ASE_0)
                        GetServerCache()->SetServerCachedInfo(this);
                }
                else
                {
                    // Pretend everything is fine until network access is confirmed
                    bScanned = true;
                    bMaybeOffline = false;
                }
                return "NoReply";
            }
        }

        return "WaitingReply";
    }
}

unsigned short CServerListItem::GetQueryPort()
{
    return endpoint.GetHostOrderPort() + SERVER_LIST_QUERY_PORT_OFFSET;
}

void CServerListItem::Query()
{
    // Performs a query according to ASE protocol
    queryReceiver.RequestQuery(IPEndpoint(endpoint.GetAddress(), GetQueryPort()));
}

bool CServerListItem::ParseQuery()
{
    SQueryInfo info = queryReceiver.GetServerResponse();
    if (!info.containingInfo)
        return false;

    // Get IP as string
    strHost = endpoint.GetAddress().ToString();

    nPing = info.pingTime;

    // Only use info from server query if master list allows it
    if ((uiMasterServerSaysRestrictions & RESTRICTION_GAME_NAME) == false)
        strGameName = info.gameName;

    if ((uiMasterServerSaysRestrictions & RESTRICTION_SERVER_NAME) == false)
        strName = info.serverName;

    if ((uiMasterServerSaysRestrictions & RESTRICTION_GAME_MODE) == false)
        strGameMode = info.gameType;

    if ((uiMasterServerSaysRestrictions & RESTRICTION_MAP_NAME) == false)
        strMap = info.mapName;

    if ((uiMasterServerSaysRestrictions & RESTRICTION_SERVER_VERSION) == false)
        strVersion = info.versionText;

    if ((uiMasterServerSaysRestrictions & RESTRICTION_PASSWORDED_FLAG) == false)
        bPassworded = info.isPassworded;

    if ((uiMasterServerSaysRestrictions & RESTRICTION_SERIALS_FLAG) == false)
        bSerials = info.serials;

    if ((uiMasterServerSaysRestrictions & RESTRICTION_PLAYER_COUNT) == false)
        nPlayers = info.players;

    if ((uiMasterServerSaysRestrictions & RESTRICTION_MAX_PLAYER_COUNT) == false)
        nMaxPlayers = info.playerSlot;

    m_iBuildType = info.buildType;
    m_iBuildNumber = info.buildNum;
    m_usHttpPort = info.httpPort;

    if ((uiMasterServerSaysRestrictions & RESTRICTION_PLAYER_LIST) == false)
        vecPlayers = info.playersPool;

    bScanned = true;

    PostChange();
    return true;
}

//////////////////////////////////////////////////////////////////////////////
//
// CServerListItemList implementation
//
//
//////////////////////////////////////////////////////////////////////////////
CServerListItemList::~CServerListItemList()
{
    dassert(m_List.size() == m_AddressMap.size());
    while (m_List.size())
        delete m_List.front();
    dassert(m_AddressMap.empty());
}

void CServerListItemList::DeleteAll()
{
    while (m_List.size())
        delete m_List.front();
    dassert(m_List.size() == m_AddressMap.size());
}

CServerListItem* CServerListItemList::Find(const IPEndpoint& endpoint)
{
    if (CServerListItem* pItem = MapFindRef(m_AddressMap, endpoint))
    {
        if (!CServerListItem::StaticIsValid(pItem))
        {
            // Bodge to fix invalid entry in map
            Remove(endpoint);
            pItem = NULL;
        }
        return pItem;
    }
    return NULL;
}

CServerListItem* CServerListItemList::AddUnique(const IPEndpoint& endpoint, bool bAtFront)
{
    if (MapContains(m_AddressMap, endpoint))
        return NULL;
    CServerListItem* pItem = new CServerListItem(endpoint, this, bAtFront);
    return pItem;
}

// Only called from CServerListItem constructor
void CServerListItemList::AddNewItem(CServerListItem* pItem, bool bAtFront)
{
    if (MapContains(m_AddressMap, pItem->endpoint))
    {
        dassert(0);
        return;
    }
    MapSet(m_AddressMap, pItem->endpoint, pItem);
    pItem->uiTieBreakPosition = 5000;
    if (!m_List.empty())
    {
        if (bAtFront)
            pItem->uiTieBreakPosition = m_List.front()->uiTieBreakPosition - 1;
        else
            pItem->uiTieBreakPosition = m_List.back()->uiTieBreakPosition + 1;
    }
    if (bAtFront)
        m_List.push_front(pItem);
    else
        m_List.push_back(pItem);
}

bool CServerListItemList::Remove(const IPEndpoint& endpoint)
{
    CServerListItem* pItem = MapFindRef(m_AddressMap, endpoint);
    if (pItem)
    {
        assert(pItem->endpoint == endpoint);
        delete pItem;
        assert(!MapFindRef(m_AddressMap, endpoint));
        return true;
    }
    return false;
}

// Only called from CServerListItem destructor
void CServerListItemList::RemoveItem(CServerListItem* pItem)
{
    dassert(m_List.size() == m_AddressMap.size());
    dassert(MapFindRef(m_AddressMap, pItem->endpoint) == pItem);

    MapRemove(m_AddressMap, pItem->endpoint);
    ListRemove(m_List, pItem);

    dassert(m_List.size() == m_AddressMap.size());
}

void CServerListItemList::OnItemChangeAddress(CServerListItem* pItem, const IPEndpoint& endpoint)
{
    // Changed?
    if (pItem->endpoint == endpoint)
        return;

    // New address free?
    if (Find(endpoint))
        return;

    // Remove old lookup
    {
        CServerListItem* pItem2 = MapFindRef(m_AddressMap, endpoint);
        assert(pItem == pItem2);
        MapRemove(m_AddressMap, endpoint);
    }

    // Add new lookup
    {
        pItem->endPointCopy = endpoint;
        pItem->endpoint = endpoint;
        MapSet(m_AddressMap, endpoint, pItem);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// CServerListItem implementation
//
//
//////////////////////////////////////////////////////////////////////////////
// Auto add to associated list
CServerListItem::CServerListItem(const IPEndpoint& endpoint, CServerListItemList* pItemList, bool bAtFront)
{
    this->endpoint = endpoint;
    m_pItemList = pItemList;
    m_bDoneTcpSend = false;
    m_bDoPostTcpQuery = false;
    m_usHttpPort = 0;
    m_ucSpecialFlags = 0;
    Init();
    if (m_pItemList)
    {
        endPointCopy = endpoint;
        m_pItemList->AddNewItem(this, bAtFront);
    }
}

// Auto removes from associated list
CServerListItem::~CServerListItem()
{
    if (m_pItemList)
    {
        // Check nothing changed
        assert(endPointCopy == endpoint);
        m_pItemList->RemoveItem(this);
    }
    MapRemove(ms_ValidServerListItemMap, this);
}

void CServerListItem::ResetForRefresh()
{
    queryReceiver.InvalidateSocket();
    bScanned = false;
    bSkipped = false;
    uiQueryRetryCount = 0;

    if (m_iDataQuality >= SERVER_INFO_QUERY)
        m_iDataQuality = SERVER_INFO_QUERY - 1;
    bMaybeOffline = false;
}

void CServerListItem::ChangeAddress(const IPEndpoint& endpoint)
{
    if (m_pItemList)
    {
        m_pItemList->OnItemChangeAddress(this, endpoint);
    }
    else
    {
        this->endpoint = endpoint;
    }
}
