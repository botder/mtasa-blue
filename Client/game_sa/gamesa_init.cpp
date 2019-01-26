/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/gamesa_init.cpp
 *  PURPOSE:     Game initialization interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#define DECLARE_PROFILER_SECTION_gamesa_init
#include "profiler/SharedUtil.Profiler.h"

CGameSA*        pGame = NULL;
CNet*           g_pNet = NULL;
CCoreInterface* g_pCore = NULL;

void internal_OUTPUT_VTABLE_DEBUG(const char* szLocation, const char* szTag, intptr_t variable)
{
    intptr_t vftable = *reinterpret_cast<intptr_t*>(variable);
    const char* szClassName = nullptr;
    switch (vftable)
    {
    case 0x871E80: szClassName = "CVehicle"; break;
    case 0x872370: szClassName = "CTrain"; break;
    case 0x871C28: szClassName = "CTrailer"; break;
    case 0x871AE8: szClassName = "CQuadBike"; break;
    case 0x867030: szClassName = "CProjectile"; break;
    case 0x86D168: szClassName = "CPlayerPed"; break;
    case 0x871948: szClassName = "CPlane"; break;
    case 0x863C40: szClassName = "CPlaceable"; break;
    case 0x863BA0: szClassName = "CPhysical"; break;
    case 0x86C358: szClassName = "CPed"; break;
    case 0x866F60: szClassName = "CObject"; break;
    case 0x8717D8: szClassName = "CMonsterTruck"; break;
    case 0x871680: szClassName = "CHeli"; break;
    case 0x866EE0: szClassName = "CHandObject"; break;
    case 0x863928: szClassName = "CEntity"; break;
    case 0x86C200: szClassName = "CEmergencyPed"; break;
    case 0x86C198: szClassName = "CDummyPed"; break;
    case 0x866E78: szClassName = "CDummyObject"; break;
    case 0x8638C0: szClassName = "CDummy"; break;
    case 0x868A60: szClassName = "CCutsceneObject"; break;
    case 0x86C120: szClassName = "CCopPed"; break;
    case 0x86C0A8: szClassName = "CCivilianPed"; break;
    case 0x8630E8: szClassName = "CCamera"; break;
    case 0x8585C8: szClassName = "CBuilding"; break;
    case 0x8721A0: szClassName = "CBoat"; break;
    case 0x871528: szClassName = "CBmx"; break;
    case 0x871360: szClassName = "CBike"; break;
    case 0x871120: szClassName = "CAutomobile"; break;
    case 0x8639B0: szClassName = "CAnimatedBuilding"; break;
    }

    static char szBuffer[256];
    sprintf(szBuffer, "%s: %s: %#x (%#x: %s)\n", szLocation, szTag, variable, vftable, szClassName ? szClassName : "unknown");
    OutputDebugStringA(szBuffer);
}

void internal_OUTPUT_VTABLE_DEBUG(const char* szLocation, const char* szTag, CVehicleSAInterface* variable)
{
    internal_OUTPUT_VTABLE_DEBUG(szLocation, szTag, reinterpret_cast<intptr_t>(variable));
}

void internal_OUTPUT_VTABLE_DEBUG(const char* szLocation, const char* szTag, CEntitySAInterface* variable)
{
    internal_OUTPUT_VTABLE_DEBUG(szLocation, szTag, reinterpret_cast<intptr_t>(variable));
}

//-----------------------------------------------------------
// This function uses the initialized data sections of the executables
// to differentiate between versions.  MUST be called at least once
// in order for proper initialization to occur.
MTAEXPORT CGame* GetGameInterface(CCoreInterface* pCore)
{
    DEBUG_TRACE("CGame * GetGameInterface()");

    g_pNet = pCore->GetNetwork();
    assert(g_pNet);

    pGame = new CGameSA;
    g_pCore = pCore;

    return (CGame*)pGame;
}

//-----------------------------------------------------------

void MemSet(void* dwDest, int cValue, uint uiAmount)
{
    if (ismemset(dwDest, cValue, uiAmount))
        return;
    SMemWrite hMem = OpenMemWrite(dwDest, uiAmount);
    memset(dwDest, cValue, uiAmount);
    CloseMemWrite(hMem);
}

void MemCpy(void* dwDest, const void* dwSrc, uint uiAmount)
{
    if (memcmp(dwDest, dwSrc, uiAmount) == 0)
        return;
    SMemWrite hMem = OpenMemWrite(dwDest, uiAmount);
    memcpy(dwDest, dwSrc, uiAmount);
    CloseMemWrite(hMem);
}

bool GetDebugIdEnabled(uint uiDebugId)
{
    return g_pCore->GetDebugIdEnabled(uiDebugId);
}

void LogEvent(uint uiDebugId, const char* szType, const char* szContext, const char* szBody, uint uiAddReportLogId)
{
    g_pCore->LogEvent(uiDebugId, szType, szContext, szBody, uiAddReportLogId);
}
