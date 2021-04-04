/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     A base class for resource files
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CChecksum.h"
#include <filesystem>

namespace mtasa
{
    class Resource;

    class ResourceFile
    {
    public:
        ResourceFile(Resource& resource) : m_resource{resource} {}

        virtual ~ResourceFile() = default;

        virtual bool Start() { return true; }
        virtual void Stop() {}

        void                         SetRelativePath(const std::filesystem::path& relativePath) { m_relativePath = relativePath; }
        const std::filesystem::path& GetRelativePath() const { return m_relativePath; }

        std::size_t      GetSize() const { return m_size; }
        const CChecksum& GetChecksum() const { return m_checksum; }

        bool CalculateFileMetaData();

    protected:
        Resource&             m_resource;
        std::filesystem::path m_relativePath;
        std::size_t           m_size = 0;
        CChecksum             m_checksum;
    };
}            // namespace mtasa
