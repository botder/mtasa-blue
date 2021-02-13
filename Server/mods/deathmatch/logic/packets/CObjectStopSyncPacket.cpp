/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Object stop sync packet class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CObjectStopSyncPacket.h"

bool CObjectStopSyncPacket::Write(NetBitStreamInterface& BitStream) const
{
    BitStream.Write(m_pObject->GetID());
    return true;
}
