/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Entity remove packet class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEntityRemovePacket.h"

bool CEntityRemovePacket::Write(NetBitStreamInterface& BitStream) const
{
    // Write each entity type then id to it
    std::vector<CElement*>::const_iterator iter = m_List.begin();

    for (; iter != m_List.end(); ++iter)
    {
        BitStream.Write((*iter)->GetID());
    }

    return m_List.size() > 0;
}
