/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     A client-side resource script file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "ResourceFile.h"
#include <string>

namespace mtasa
{
    class ClientResourceScriptFile final : public ResourceFile
    {
    public:
        ClientResourceScriptFile(Resource& resource) : ResourceFile(resource, ResourceFileType::CLIENT_SCRIPT) {}

        void SetIsCachable(bool isCachable) { m_isCachable = isCachable; }
        bool IsCachable() const { return m_isCachable; }

        const std::string& GetCompressedSource() const { return m_compressedSource; }

    private:
        bool        m_isCachable = true;
        std::string m_compressedSource;
    };
}            // namespace mtasa
