/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     A server-side XML configuration resource file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "ResourceFile.h"

class CXMLNode;

namespace mtasa
{
    class ResourceConfigFile final : public ResourceFile
    {
    public:
        using ResourceFile::ResourceFile;

    private:
        CXMLNode* m_xmlFile = nullptr;
        CXMLNode* m_rootNode = nullptr;
    };
}            // namespace mtasa
