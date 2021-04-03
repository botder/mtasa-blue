/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource client-side scripts packet class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CResourceClientScriptsPacket.h"
#include "Resource.h"

bool CResourceClientScriptsPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (m_scripts.empty() || m_scripts.size() > 0xFFFF)
        return false;

    BitStream.Write(m_resource.GetRemoteIdentifier());

    auto scriptCount = static_cast<std::uint16_t>(m_scripts.size());
    BitStream.Write(scriptCount);

    bool writeResourcePath = (BitStream.Version() >= 0x50);

    for (const Script& script : m_scripts)
    {
        if (script.relativePath.size() > 0xFFFF || script.sourceCode.size() > 0xFFFFFFFF)
            return false;

        if (writeResourcePath)
        {
            auto filePathLength = static_cast<std::uint16_t>(script.relativePath.size());
            BitStream.Write(filePathLength);

            if (filePathLength > 0)
                BitStream.Write(script.relativePath.data(), filePathLength);
        }

        auto sourceCodeLength = static_cast<unsigned int>(script.sourceCode.size());
        BitStream.Write(sourceCodeLength);
        BitStream.Write(script.sourceCode.data(), sourceCodeLength);
    }

    return true;
}
