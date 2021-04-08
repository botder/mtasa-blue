/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     CRC-32 checksum and MD5 hash for a resource file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ResourceFileChecksum.h"
#include <optional>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/crc.h>
#include <cryptopp/md5.h>
#include <cryptopp/files.h>
#include <cryptopp/channels.h>

namespace fs = std::filesystem;

namespace mtasa
{
    using CRC32_MD5 = std::pair<std::uint32_t, std::array<std::uint8_t, 16>>;

    std::optional<CRC32_MD5> ComputeHashes(const fs::path& filePath);

    bool ResourceFileChecksum::Compute(const fs::path& filePath)
    {
        // Do not update the checksum if the file's last write time didn't change
        std::error_code    errorCode;
        fs::file_time_type lastWriteTime = fs::last_write_time(filePath, errorCode);

        if (!errorCode && lastWriteTime == m_lastWriteTime)
            return true;

        std::optional<CRC32_MD5> hashes = ComputeHashes(filePath);

        if (hashes.has_value())
        {
            m_crc = hashes->first;
            m_md5 = hashes->second;
            m_lastWriteTime = lastWriteTime;
            return true;
        }
        else
        {
            m_crc = 0;
            m_md5 = {};
            return false;
        }
    }

    bool ResourceFileChecksum::HasChanged(const std::filesystem::path& filePath) const
    {
        std::error_code    errorCode;
        fs::file_time_type lastWriteTime = fs::last_write_time(filePath, errorCode);

        if (errorCode || lastWriteTime != m_lastWriteTime)
            return true;

        std::optional<CRC32_MD5> hashes = ComputeHashes(filePath);

        if (!hashes.has_value())
            return true;

        return hashes->first != m_crc || hashes->second != m_md5;
    }

    std::optional<CRC32_MD5> ComputeHashes(const fs::path& filePath)
    {
        std::string crcBytes;
        std::string md5Bytes;

        try
        {
            // Create hash storage variables
            CryptoPP::CRC32     crc;
            CryptoPP::Weak::MD5 md5;

            // Create hash filters
            CryptoPP::HashFilter f1(crc, new CryptoPP::StringSink(crcBytes));
            CryptoPP::HashFilter f2(md5, new CryptoPP::StringSink(md5Bytes));

            // Create hash filter combinator
            CryptoPP::ChannelSwitch channel;
            channel.AddDefaultRoute(f1);
            channel.AddDefaultRoute(f2);

            // Pipe file content through our channel
            CryptoPP::FileSource fileSource(filePath.c_str(), true, new CryptoPP::Redirector(channel));
        }
        catch (const CryptoPP::Exception&)
        {
            return std::nullopt;
        }

        std::uint32_t                crc = 0;
        std::array<std::uint8_t, 16> md5;

        if (crcBytes.size() != sizeof(crc) || md5Bytes.size() != md5.size())
            return std::nullopt;

        std::copy(crcBytes.begin(), crcBytes.end(), reinterpret_cast<char*>(&crc));
        std::copy(md5Bytes.begin(), md5Bytes.end(), reinterpret_cast<char*>(md5.data()));
        return std::make_pair(crc, md5);
    }
}            // namespace mtasa
