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
