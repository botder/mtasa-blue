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
