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

        virtual bool Start()
        {
            m_isRunning = true;
            return true;
        }

        virtual void Stop()
        {
            m_isRunning = false;
        }

        ResourceFileType GetType() const { return m_type; }

        bool IsRunning() const { return m_isRunning; }

        void SetRelativePath(const std::filesystem::path& relativePath)
        {
            m_relativePath = relativePath;
            m_name = relativePath.generic_string();
        }

        const std::filesystem::path& GetRelativePath() const { return m_relativePath; }

        const std::string& GetName() const { return m_name; }
        std::size_t        GetSize() const { return m_size; }
        const CChecksum&   GetChecksum() const { return m_checksum; }

        bool CalculateFileMetaData();

    protected:
        Resource&             m_resource;
        ResourceFileType      m_type;
        bool                  m_isRunning = false;
        std::filesystem::path m_relativePath;
        std::string           m_name;
        std::size_t           m_size = 0;
        CChecksum             m_checksum;
    };
}            // namespace mtasa
