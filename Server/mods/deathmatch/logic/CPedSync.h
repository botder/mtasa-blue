/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Ped entity synchronization class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPlayerManager.h"
#include "CPedManager.h"
#include "packets/CPedSyncPacket.h"

class CPedSync
{
public:
    CPedSync(CPlayerManager* pPlayerManager, CPedManager* pPedManager);

    void DoPulse();
    bool ProcessPacket(CPacket& Packet);

    void OverrideSyncer(CPed* pPed, CPlayer* pPlayer);

private:
    void     Update();
    void     UpdatePed(CPed* pPed);
    void     FindSyncer(CPed* pPed);
    CPlayer* FindPlayerCloseToPed(CPed* pPed, float fMaxDistance);

    void StartSync(CPlayer* pPlayer, CPed* pPed);
    void StopSync(CPed* pPed);

    void Packet_PedSync(CPedSyncPacket& Packet);

    CPlayerManager* m_pPlayerManager;
    CPedManager*    m_pPedManager;

    CElapsedTime m_UpdateTimer;
};
