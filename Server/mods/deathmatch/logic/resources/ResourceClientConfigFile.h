/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     A client-side resource configuration file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "ResourceFile.h"

namespace mtasa
{
    class ResourceClientConfigFile final : public ResourceFile
    {
    public:
        ResourceClientConfigFile(Resource& resource) : ResourceFile(resource, ResourceFileType::CLIENT_CONFIG)
        {
            m_usingClientCache = true;
        }
    };
}            // namespace mtasa
