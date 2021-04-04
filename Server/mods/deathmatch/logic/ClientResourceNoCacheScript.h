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
