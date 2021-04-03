/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource client-side scripts packet class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <vector>
#include <string_view>

namespace mtasa
{
    class Resource;
}

class CResourceClientScriptsPacket final : public CPacket
{
public:
    struct Script
    {
        std::string_view relativePath;
        std::string_view sourceCode;
    };

    CResourceClientScriptsPacket(mtasa::Resource& resource) : m_resource{resource} {}

    ePacketID     GetPacketID() const { return PACKET_ID_RESOURCE_CLIENT_SCRIPTS; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    void AddScript(Script&& script) { m_scripts.push_back(std::move(script)); }

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    mtasa::Resource&    m_resource;
    std::vector<Script> m_scripts;
};
