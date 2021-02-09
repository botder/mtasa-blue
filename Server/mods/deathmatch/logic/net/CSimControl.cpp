/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "SimHeaders.h"

namespace
{
    bool               ms_bEnabled = false;
    bool               ms_bEnableRequest = false;
    CNetServerBuffer*  ms_pSimNetServerBuffer = NULL;
    CSimPlayerManager* ms_pSimPlayerManager = NULL;
}            // namespace

///////////////////////////////////////////////////////////////
//
// CSimControl::Startup
//
//
//
///////////////////////////////////////////////////////////////
void CSimControl::Startup()
{
    ms_pSimPlayerManager = new CSimPlayerManager();

    // Check packet flags are what we are expecting
    dassert(CPlayerPuresyncPacket().HasSimHandler());
    dassert(CVehiclePuresyncPacket().HasSimHandler());
    dassert(CKeysyncPacket().HasSimHandler());
    dassert(CBulletsyncPacket().HasSimHandler());
}

///////////////////////////////////////////////////////////////
//
// CSimControl::Shutdown
//
//
//
///////////////////////////////////////////////////////////////
void CSimControl::Shutdown()
{
    EnableSimSystem(false);
    SAFE_DELETE(ms_pSimPlayerManager);
}

///////////////////////////////////////////////////////////////
//
// CSimControl::EnableSimSystem
//
// Turn on and off here
// Not applied until the next pulse unless bApplyNow is set
//
///////////////////////////////////////////////////////////////
void CSimControl::EnableSimSystem(bool bEnable, bool bApplyNow)
{
    ms_bEnableRequest = bEnable;
    if (bApplyNow)
        DoPulse();
}

///////////////////////////////////////////////////////////////
//
// CSimControl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CSimControl::DoPulse()
{
    if (ms_bEnableRequest == ms_bEnabled)
        return;
    ms_bEnabled = ms_bEnableRequest;

    if (ms_bEnabled)
    {
        // Startup NetServerBuffer
        assert(!ms_pSimNetServerBuffer);
        ms_pSimNetServerBuffer = new CNetServerBuffer(ms_pSimPlayerManager);

        // Replace g_pNetServer
        g_pNetServer = ms_pSimNetServerBuffer;

        // Replace packet handler
        ms_pSimNetServerBuffer->RegisterPacketHandler(CGame::StaticProcessPacket);

        // Let the pulsing begin
        ms_pSimNetServerBuffer->SetAutoPulseEnabled(true);
    }
    else
    {
        // Stop the sync thread from doing anything by itself
        ms_pSimNetServerBuffer->SetAutoPulseEnabled(false);

        // Restore g_pNetServer
        g_pNetServer = g_pRealNetServer;

        // Restore packet handler - This is blocking so will drain the outgoing queue
        ms_pSimNetServerBuffer->RegisterPacketHandler(NULL);
        g_pRealNetServer->RegisterPacketHandler(CGame::StaticProcessPacket);

        // Drain the incoming queue
        ms_pSimNetServerBuffer->ProcessIncoming();

        // Kaboooom
        SAFE_DELETE(ms_pSimNetServerBuffer);
    }
}

///////////////////////////////////////////////////////////////
//
// CSimControl::IsSimSystemEnabled
//
// Check if sim system is on
//
///////////////////////////////////////////////////////////////
bool CSimControl::IsSimSystemEnabled()
{
    return ms_bEnabled;
}

///////////////////////////////////////////////////////////////
//
// CSimControl::AddSimPlayer
//
// Add a joining player
//
///////////////////////////////////////////////////////////////
void CSimControl::AddSimPlayer(CPlayer* pPlayer)
{
    ms_pSimPlayerManager->AddSimPlayer(pPlayer);
}

///////////////////////////////////////////////////////////////
//
// CSimControl::RemoveSimPlayer
//
// Remove a leaving player
//
///////////////////////////////////////////////////////////////
void CSimControl::RemoveSimPlayer(CPlayer* pPlayer)
{
    ms_pSimPlayerManager->RemoveSimPlayer(pPlayer);
}

///////////////////////////////////////////////////////////////
//
// CSimControl::UpdateSimPlayer
//
// Update a player at pure sync (and other) times
//
///////////////////////////////////////////////////////////////
void CSimControl::UpdateSimPlayer(CPlayer* pPlayer)
{
    ms_pSimPlayerManager->UpdateSimPlayer(pPlayer);
}
