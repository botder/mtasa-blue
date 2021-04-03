/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource start packet class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <vector>

namespace mtasa
{
    class Resource;
}

class CResourceStartPacket final : public CPacket
{
public:
    enum class ClientFileType
    {
        FILE,
        CONFIG,
        SCRIPT,
    };

    struct ClientFile
    {
        ClientFileType      type = ClientFileType::FILE;
        std::string_view    relativePath;
        CChecksum           checksum;
        double              approximateSize = 0;
        std::optional<bool> isOptional;
    };

    CResourceStartPacket(mtasa::Resource& resource) : m_resource{resource} {}

    ePacketID     GetPacketID() const { return PACKET_ID_RESOURCE_START; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    void AddClientFile(ClientFile&& file) { m_files.push_back(std::move(file)); }

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    mtasa::Resource&        m_resource;
    std::vector<ClientFile> m_files;
};
