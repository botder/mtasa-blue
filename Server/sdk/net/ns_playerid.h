/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/net/ns_playerid.h
 *  PURPOSE:     Net server player ID interface
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <MTAPlatform.h>
#include <mtasa/IPEndpoint.h>

class SNetExtraInfo : public CRefCountable
{
protected:
    SNetExtraInfo(const SNetExtraInfo&) = default;
    SNetExtraInfo& operator=(const SNetExtraInfo&) = default;

public:
    ZERO_ON_NEW
    SNetExtraInfo() = default;

    bool m_bHasPing = false;
    uint m_uiPing = 0;
};

using NetServerPlayerID = mtasa::IPEndpoint;
