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

#include "ResourceFileChecksum.h"
#include <filesystem>
#include <cstdint>

namespace mtasa
{
    class Resource;

    enum class ResourceFileType : std::uint8_t
    {
        CLIENT_FILE,
        CLIENT_SCRIPT,
        CLIENT_CONFIG,
        SERVER_MAP,
        SERVER_HTTP,
        SERVER_CONFIG,
        SERVER_SCRIPT,
    };

    class ResourceFile
    {
    public:
        ResourceFile(Resource& resource, ResourceFileType type) : m_resource{resource}, m_type{type} {}

        virtual ~ResourceFile() = default;

        Resource&       GetResource() { return m_resource; }
        const Resource& GetResource() const { return m_resource; }

        virtual bool Start();
        virtual void Stop();

        ResourceFileType GetType() const { return m_type; }

        bool IsRunning() const { return m_isRunning; }

        void SetRelativePath(const std::filesystem::path& relativePath)
        {
            m_relativePath = relativePath;
            m_name = relativePath.generic_string();
        }

        const std::filesystem::path& GetRelativePath() const { return m_relativePath; }

        const std::string& GetName() const { return m_name; }

        bool HasChanged() const;

        bool IsUsingClientCache() const { return m_usingClientCache; }

        std::filesystem::path GetSourceFilePath() const;
        std::filesystem::path GetCacheFilePath() const;

    public:
        bool ComputeSourceFileMetaData();
        bool ComputeCacheFileMetaData();

        bool           SourceFileExists() const { return m_sourceFileExists; }
        std::uintmax_t GetSize() const { return m_sourceFileSize; }

        const ResourceFileChecksum& GetSourceChecksum() const { return m_sourceChecksum; }
        const ResourceFileChecksum& GetCacheChecksum() const { return m_cacheChecksum; }

    protected:
        Resource&             m_resource;
        ResourceFileType      m_type;
        std::string           m_name;
        std::filesystem::path m_relativePath;
        bool                  m_isRunning = false;
        bool                  m_sourceFileExists = true;
        bool                  m_usingClientCache = false;
        std::uintmax_t        m_sourceFileSize = 0;
        ResourceFileChecksum  m_sourceChecksum;
        ResourceFileChecksum  m_cacheChecksum;
    };
}            // namespace mtasa
