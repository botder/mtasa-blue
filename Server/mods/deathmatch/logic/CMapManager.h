/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMapManager.h
 *  PURPOSE:     Map manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <set>
#include <vector>

class CVector;
class CDummy;
class CElement;
class CPerPlayerEntity;
class CPlayer;
class CTeam;
class CClock;
class CEvents;
class CGroups;
class CTeamManager;
class CVehicleManager;
class CMarkerManager;
class CRadarAreaManager;
class CPlayerManager;
class CPedManager;
class CPickupManager;
class CColManager;
class CWaterManager;
class CObjectManager;
class CElementDeleter;
class CBlipManager;
class CBlendedWeather;
class CScriptDebugging;
class CEntityAddPacket;
class CXMLNode;

namespace mtasa
{
    class Resource;
}

class CMapManager
{
public:
    CMapManager(CBlipManager* pBlipManager, CObjectManager* pObjectManager, CPickupManager* pPickupManager, CPlayerManager* pPlayerManager,
                CRadarAreaManager* pRadarAreaManager, CMarkerManager* pMarkerManager, CVehicleManager* pVehicleManager, CTeamManager* pTeamManager,
                CPedManager* pPedManager, CColManager* pColManager, CWaterManager* pWaterManager, CClock* pClock,
                CGroups* pGroups, CEvents* pEvents, CScriptDebugging* pScriptDebugging, CElementDeleter* pElementDeleter);
    ~CMapManager();

    CBlendedWeather* GetWeather() { return m_pBlendedWeather; }

    void DoPulse();

    CElement* LoadMapData(mtasa::Resource& resource, CElement& Parent, CXMLNode& Node);

    void BroadcastMapInformation();
    void SendMapInformation(CPlayer& Player);
    void SendBlips(CPlayer& Player);
    void SendPerPlayerEntities(CPlayer& Player);

    void BroadcastResourceElements(CElement* pResourceElement, CElementGroup* pElementGroup);
    void BroadcastElementChildren(CElement* pElement, CEntityAddPacket& Packet, std::vector<CPerPlayerEntity*>& pPerPlayerList,
                                  std::set<CElement*>& outDoneElements);
    void BroadcastElement(CElement* pElement, CEntityAddPacket& Packet, std::vector<CPerPlayerEntity*>& pPerPlayerList);

    void OnPlayerJoin(CPlayer& Player);
    void OnPlayerQuit(CPlayer& Player);

    CDummy* GetRootElement() const { return m_pRootElement; }

    CClock* GetServerClock() { return m_pServerClock; }

    void SpawnPlayer(CPlayer& Player, const CVector& vecPosition, float fRotation, unsigned short usModel, unsigned char ucInterior = 0,
                     unsigned short usDimension = 0, CTeam* pTeam = NULL);

    void DoRespawning();
    void DoPickupRespawning();
    void DoVehicleRespawning();

private:
    void SetUpVisibleToReferences(CElement* pElement);
    void ProcessVisibleToData(CPerPlayerEntity& Entity);
    bool ParseVisibleToData(CPerPlayerEntity& Entity, char* szData);

    CElement* LoadNode(mtasa::Resource& resource, CXMLNode& Node, CElement* pParent, std::vector<CElement*>* pAdded, bool bIsDuringStart);
    bool      LoadSubNodes(mtasa::Resource& resource, CXMLNode& Node, CElement* pParent, std::vector<CElement*>* pAdded, bool bIsDuringStart);
    bool      HandleNode(mtasa::Resource& resource, CXMLNode& Node, CElement* pParent, std::vector<CElement*>* pAdded, bool bIsDuringStart, CElement** pCreated);
    void      LinkupElements();

    CBlipManager*      m_pBlipManager;
    CObjectManager*    m_pObjectManager;
    CPickupManager*    m_pPickupManager;
    CPlayerManager*    m_pPlayerManager;
    CRadarAreaManager* m_pRadarAreaManager;
    CMarkerManager*    m_pMarkerManager;
    CVehicleManager*   m_pVehicleManager;
    CTeamManager*      m_pTeamManager;
    CPedManager*       m_pPedManager;
    CColManager*       m_pColManager;
    CWaterManager*     m_pWaterManager;
    CClock*            m_pServerClock;
    CGroups*           m_pGroups;
    CEvents*           m_pEvents;
    CScriptDebugging*  m_pScriptDebugging;
    CElementDeleter*   m_pElementDeleter;

    CDummy* m_pRootElement;

    long long m_llLastRespawnTime = 0;

    CBlendedWeather* m_pBlendedWeather;
};
