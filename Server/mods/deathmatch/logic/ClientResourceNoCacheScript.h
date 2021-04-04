/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     A client-side no-cache resource script file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "ResourceFile.h"
#include <string>

namespace mtasa
{
    class ClientResourceNoCacheScript final : public ResourceFile
    {
    public:
        using ResourceFile::ResourceFile;

        const std::string& GetCompressedSource() const { return m_compressedSource; }

    private:
        std::string m_compressedSource;
    };
}            // namespace mtasa
