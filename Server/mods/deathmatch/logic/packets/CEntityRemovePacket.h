/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Entity remove packet class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <vector>

class CEntityRemovePacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const { return PACKET_ID_ENTITY_REMOVE; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

    void Add(class CElement* pElement) { m_List.push_back(pElement); };
    void Clear() { m_List.clear(); };

private:
    std::vector<class CElement*> m_List;
};
