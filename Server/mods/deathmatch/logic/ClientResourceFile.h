/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     A client-side resource file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "ResourceFile.h"

namespace mtasa
{
    class ClientResourceFile final : public ResourceFile
    {
    public:
        using ResourceFile::ResourceFile;

        void SetIsOptional(bool isOptional) { m_isOptional = isOptional; }
        bool IsOptional() const { return m_isOptional; }

    private:
        bool m_isOptional = false;
    };
}            // namespace mtasa
