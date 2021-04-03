/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource stop packet handler class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <cstdint>

class CResourceStopPacket final : public CPacket
{
public:
    CResourceStopPacket(std::uint16_t usID) : m_usID{usID} {}

    ePacketID     GetPacketID() const { return PACKET_ID_RESOURCE_STOP; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const
    {
        BitStream.Write(m_usID);
        return true;
    }

private:
    std::uint16_t m_usID;
};
