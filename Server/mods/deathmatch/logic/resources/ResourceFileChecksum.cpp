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

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/crc.h>
#include <cryptopp/md5.h>
#include <cryptopp/files.h>
#include <cryptopp/channels.h>

namespace fs = std::filesystem;

namespace mtasa
{
    bool ResourceFileChecksum::Calculate(const fs::path& filePath)
    {
        // Do not update the checksum if the file's last write time didn't change
        std::error_code    errorCode;
        fs::file_time_type lastWriteTime = fs::last_write_time(filePath, errorCode);

        if (!errorCode && lastWriteTime == m_lastWriteTime)
            return true;

        // Reset the current checksums
        m_crc = 0;
        m_md5 = {};

        // Output strings for hash filters
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
            return false;
        }

        if (crcBytes.size() != sizeof(m_crc) || md5Bytes.size() != m_md5.size())
            return false;

        std::copy(crcBytes.begin(), crcBytes.end(), reinterpret_cast<char*>(&m_crc));
        std::copy(md5Bytes.begin(), md5Bytes.end(), reinterpret_cast<char*>(m_md5.data()));
        m_lastWriteTime = lastWriteTime;
        return true;
    }
}            // namespace mtasa
