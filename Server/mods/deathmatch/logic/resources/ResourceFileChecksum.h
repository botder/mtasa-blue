/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     CRC-32 checksum and MD5 hash for a resource file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <array>
#include <cstdint>
#include <filesystem>

namespace mtasa
{
    class ResourceFileChecksum
    {
    public:
        void Reset()
        {
            m_crc = 0;
            m_md5 = {};
            m_lastWriteTime = {};
        }

        bool IsCalculated() const { return m_crc != 0; }

        bool Calculate(const std::filesystem::path& filePath);

        bool operator==(const ResourceFileChecksum& other)
        {
            return m_crc == other.m_crc && m_md5 == other.m_md5;
        }

        std::uint32_t                       GetCRC() const { return m_crc; }
        const std::array<std::uint8_t, 16>& GetMD5() const { return m_md5; }

    private:
        std::uint32_t                   m_crc = 0;
        std::array<std::uint8_t, 16>    m_md5;
        std::filesystem::file_time_type m_lastWriteTime;
    };
}
