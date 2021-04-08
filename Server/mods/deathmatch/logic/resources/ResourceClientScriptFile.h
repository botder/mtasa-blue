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

namespace mtasa
{
    class ResourceClientScriptFile final : public ResourceFile
    {
    public:
        ResourceClientScriptFile(Resource& resource, bool isClientCachable) : ResourceFile(resource, ResourceFileType::CLIENT_SCRIPT)
        {
            m_usingClientCache = isClientCachable;
        }

        const std::string& GetCompressedSource() const { return m_compressedSource; }

    private:
        std::string m_compressedSource;
    };
}            // namespace mtasa
