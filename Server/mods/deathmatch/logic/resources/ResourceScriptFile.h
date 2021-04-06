/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     A server-side script resource file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "ResourceFile.h"

namespace mtasa
{
    class ResourceScriptFile final : public ResourceFile
    {
    public:
        ResourceScriptFile(Resource& resource) : ResourceFile(resource, ResourceFileType::SERVER_SCRIPT) {}

        bool Start() override;
    };
}            // namespace mtasa
