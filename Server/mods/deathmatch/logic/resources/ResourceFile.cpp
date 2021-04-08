/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     A base class for resource files
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ResourceFile.h"
#include "Resource.h"
#include "ResourceFileChecksum.h"

namespace fs = std::filesystem;

namespace mtasa
{
    bool ResourceFile::Start()
    {
        if (m_isRunning)
            return false;

        if (m_usingClientCache)
        {
            fs::path cacheFile{GetCacheFilePath()};

            // Validate the cache filename first
            if (!g_pRealNetServer->ValidateHttpCacheFileName(cacheFile.generic_string().c_str()))
            {
                std::error_code ignore;
                fs::remove(cacheFile, ignore);
                return false;
            }

            // Create the cache file, if it doesn't exist
            std::error_code errorCode;

            if (!fs::exists(cacheFile, errorCode))
            {
                // Before copying, ensure every directory including the parent exist
                fs::create_directories(cacheFile.parent_path(), errorCode);

                if (errorCode)
                    return false;

                if (!fs::copy_file(GetSourceFilePath(), cacheFile, errorCode))
                    return false;
            }

            m_cacheChecksum.Compute(cacheFile);

            // Compare the source and cache file checksums and, if they differ, copy the source file to the cache (again)
            if (m_cacheChecksum != m_sourceChecksum)
            {
                if (!fs::copy_file(GetSourceFilePath(), cacheFile, errorCode))
                    return false;

                if (!m_cacheChecksum.Compute(cacheFile))
                    return false;

                if (m_cacheChecksum != m_sourceChecksum)
                    return false;
            }
        }

        m_isRunning = true;
        return true;
    }

    void ResourceFile::Stop()
    {
        if (!m_isRunning)
            return;

        m_cacheChecksum.Reset();
        m_isRunning = false;
    }

    bool ResourceFile::HasChanged() const
    {
        if (m_sourceChecksum.HasChanged(GetSourceFilePath()))
            return true;

        return m_usingClientCache && m_cacheChecksum.HasChanged(GetCacheFilePath());
    }

    std::filesystem::path ResourceFile::GetSourceFilePath() const
    {
        return m_resource.GetSourceDirectory() / m_relativePath;
    }

    std::filesystem::path ResourceFile::GetCacheFilePath() const
    {
        return m_resource.GetClientCacheDirectory() / m_relativePath;
    }

    bool ResourceFile::ComputeSourceFileMetaData()
    {
        fs::path        sourceFile{GetSourceFilePath()};
        std::error_code errorCode;

        m_sourceFileSize = fs::file_size(sourceFile, errorCode);
        m_sourceFileExists = (errorCode.value() == 0);

        if (!m_sourceFileExists)
        {
            m_sourceFileSize = 0;
            m_sourceChecksum.Reset();
            return false;
        }

        return m_sourceChecksum.Compute(sourceFile);
    }
}            // namespace mtasa
