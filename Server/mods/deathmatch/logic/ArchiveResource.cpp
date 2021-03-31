/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Provide a resource from an archive file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ArchiveResource.h"

namespace fs = std::filesystem;

namespace mtasa
{
    enum class DecompressionError
    {
        NONE,
        CREATE_OUTPUT_PARENT_DIRECTORY,
        CREATE_OUTPUT_FILE,
        WRITE_OUTPUT_FILE,
        OPEN_ARCHIVE_FILE,
        READ_ARCHIVE_FILE,
    };

    static unzFile            OpenArchiveFile(const fs::path& filePath);
    static DecompressionError DecompressFile(unzFile zipHandle, const unz_file_info& fileInfo, const fs::path& outputBaseDirectory,
                                             const std::string_view& fileName, std::byte* buffer, unsigned int bufferSize);
    static std::string_view   DecompressionErrorToString(DecompressionError decompressionError);

    bool ArchiveResource::Load()
    {
        return PreProcessArchive() && Resource::Load();
    }

    bool ArchiveResource::Start()
    {
        return DecompressArchive() && Resource::Start();
    }

    bool ArchiveResource::ContainsSourceFile(const fs::path& relativePath) const
    {
        return m_archiveSourceFiles.find(relativePath) != m_archiveSourceFiles.end();
    }

    bool ArchiveResource::PreProcessArchive()
    {
        unzFile zipHandle = OpenArchiveFile(m_sourceArchive);

        if (zipHandle == nullptr)
        {
            m_lastError = SString("Couldn't open archive file for resource '%.*s'", m_name.size(), m_name.c_str());
            CLogger::ErrorPrintf(m_lastError.c_str());
            return false;
        }

        std::unique_ptr<unzFile, decltype(&unzClose)> zipCloser{reinterpret_cast<unzFile*>(zipHandle), &unzClose};

        if (unzGoToFirstFile(zipHandle) != UNZ_OK)
        {
            m_lastError = SString("Failed to process archive file for resource '%.*s': first file not found", m_name.size(), m_name.c_str());
            CLogger::ErrorPrintf(m_lastError.c_str());
            return false;
        }

        constexpr uLong fileNameBufferSize = 65535;
        auto            fileNameBuffer = std::make_unique<char[]>(fileNameBufferSize);

        bool hasMetaFile = false;

        do
        {
            unz_file_info fileInfo;
            memset(&fileInfo, 0, sizeof(fileInfo));

            if (unzGetCurrentFileInfo(zipHandle, &fileInfo, fileNameBuffer.get(), fileNameBufferSize, nullptr, 0, nullptr, 0) != UNZ_OK)
                return false;

            std::string_view fileName{fileNameBuffer.get(), fileInfo.size_filename};

            if (fileName.back() == '/')
                continue;

            if (fileName == "meta.xml"sv)
            {
                hasMetaFile = true;

                constexpr unsigned int bufferSize = 8192;
                auto                   buffer = std::make_unique<std::byte[]>(bufferSize);

                DecompressionError decompressionError = DecompressFile(zipHandle, fileInfo, m_sourceDirectory, fileName, buffer.get(), bufferSize);

                if (decompressionError != DecompressionError::NONE)
                {
                    std::string_view error = DecompressionErrorToString(decompressionError);

                    m_lastError = SString("Processing archive file 'meta.xml' for resource '%.*s' failed: %.*s", m_name.size(), m_name.c_str(), error.size(),
                                          error.data());

                    CLogger::ErrorPrintf(m_lastError.c_str());
                    return false;
                }
            }

            m_archiveSourceFiles.emplace(fileName);
        } while (unzGoToNextFile(zipHandle) == UNZ_OK);

        if (!hasMetaFile)
        {
            m_lastError = "Couldn't find meta.xml file for resource '"s + m_name + "'\n"s;
            CLogger::ErrorPrintf(m_lastError.c_str());
            return false;
        }

        return true;
    }

    bool ArchiveResource::DecompressArchive()
    {
        unzFile zipHandle = OpenArchiveFile(m_sourceArchive);

        if (zipHandle == nullptr)
        {
            m_lastError = SString("Couldn't open archive file for resource '%.*s'", m_name.size(), m_name.c_str());
            CLogger::ErrorPrintf(m_lastError.c_str());
            return false;
        }

        std::unique_ptr<unzFile, decltype(&unzClose)> zipCloser{reinterpret_cast<unzFile*>(zipHandle), &unzClose};

        if (unzGoToFirstFile(zipHandle) != UNZ_OK)
        {
            m_lastError = SString("Failed to process archive file for resource '%.*s': first file not found", m_name.size(), m_name.c_str());
            CLogger::ErrorPrintf(m_lastError.c_str());
            return false;
        }

        constexpr uLong fileNameBufferSize = 65535;
        auto            fileNameBuffer = std::make_unique<char[]>(fileNameBufferSize);

        constexpr unsigned int bufferSize = 8192;
        auto                   buffer = std::make_unique<std::byte[]>(bufferSize);

        do
        {
            unz_file_info fileInfo;
            memset(&fileInfo, 0, sizeof(fileInfo));

            if (unzGetCurrentFileInfo(zipHandle, &fileInfo, fileNameBuffer.get(), fileNameBufferSize, nullptr, 0, nullptr, 0) != UNZ_OK)
                return false;

            std::string_view fileName{fileNameBuffer.get(), fileInfo.size_filename};

            if (fileName.back() == '/')
                continue;

            DecompressionError decompressionError = DecompressFile(zipHandle, fileInfo, m_sourceDirectory, fileName, buffer.get(), bufferSize);

            if (decompressionError != DecompressionError::NONE)
            {
                std::string_view error = DecompressionErrorToString(decompressionError);

                m_lastError = SString("Processing archive file '%.*s' for resource '%.*s' failed: %.*s", fileName.size(), fileName.data(), m_name.size(),
                                      m_name.c_str(), error.size(), error.data());

                CLogger::ErrorPrintf(m_lastError.c_str());
                return false;
            }
        } while (unzGoToNextFile(zipHandle) == UNZ_OK);

        return true;
    }

    static unzFile OpenArchiveFile(const fs::path& filePath)
    {
#ifdef WIN32
        zlib_filefunc_def ffunc;
        fill_win32_filefunc(&ffunc);
        return unzOpen2(filePath.string().c_str(), &ffunc);
#else
        return unzOpen(filePath.c_str());
#endif
    }

    static DecompressionError DecompressFile(unzFile zipHandle, const unz_file_info& fileInfo, const fs::path& outputBaseDirectory,
                                             const std::string_view& fileName, std::byte* buffer, unsigned int bufferSize)
    {
        fs::path outputFilePath = outputBaseDirectory / fileName;

        struct tm zipFileWriteTime;
        memset(&zipFileWriteTime, 0, sizeof(zipFileWriteTime));
        zipFileWriteTime.tm_sec = fileInfo.tmu_date.tm_sec;
        zipFileWriteTime.tm_min = fileInfo.tmu_date.tm_min;
        zipFileWriteTime.tm_hour = fileInfo.tmu_date.tm_hour;
        zipFileWriteTime.tm_mday = fileInfo.tmu_date.tm_mday;
        zipFileWriteTime.tm_mon = fileInfo.tmu_date.tm_mon;
        zipFileWriteTime.tm_year = fileInfo.tmu_date.tm_year - 1900;
        zipFileWriteTime.tm_isdst = -1;

        std::error_code errorCode;

        if (fs::is_regular_file(outputFilePath, errorCode))
        {
            // Compare file write time (in epoch) to avoid heavy CRC checksum calculation
            struct tm fileWriteTime;

            if (GetFileLastWriteTime(outputFilePath.c_str(), fileWriteTime))
            {
                time_t zipEpoch = mktime(&zipFileWriteTime);
                time_t outputEpoch = mktime(&fileWriteTime);

                if (zipEpoch != -1 && outputEpoch != -1 && difftime(zipEpoch, outputEpoch) == 0)
                    return DecompressionError::NONE;
            }

            unsigned long crc = CRCGenerator::GetCRCFromFile(outputFilePath.string().c_str());

            if (crc == fileInfo.crc)
            {
                SetFileLastWriteTime(outputFilePath.c_str(), zipFileWriteTime);
                return DecompressionError::NONE;
            }
        }

        if (fs::create_directories(outputFilePath.parent_path(), errorCode); errorCode)
            return DecompressionError::CREATE_OUTPUT_PARENT_DIRECTORY;

        FILE* outputStream = File::Fopen(outputFilePath.string().c_str(), "wb");

        if (outputStream == nullptr)
            return DecompressionError::CREATE_OUTPUT_FILE;

        std::unique_ptr<FILE, decltype(&fclose)> fileCloser{outputStream, &fclose};

        if (unzOpenCurrentFile(zipHandle) != UNZ_OK)
            return DecompressionError::OPEN_ARCHIVE_FILE;

        std::unique_ptr<unzFile, decltype(&unzCloseCurrentFile)> currentFileCloser{reinterpret_cast<unzFile*>(zipHandle), &unzCloseCurrentFile};

        for (;;)
        {
            int numBytes = unzReadCurrentFile(zipHandle, buffer, bufferSize);

            if (numBytes < 0)
                return DecompressionError::READ_ARCHIVE_FILE;

            if (numBytes == 0)
                break;

            if (fwrite(buffer, numBytes, 1, outputStream) != 1)
                return DecompressionError::WRITE_OUTPUT_FILE;
        }

        // NOTE(botder): We must flush and close the file before applying the last write time
        fileCloser.reset();

        SetFileLastWriteTime(outputFilePath.c_str(), zipFileWriteTime);
        return DecompressionError::NONE;
    }

    static std::string_view DecompressionErrorToString(DecompressionError decompressionError)
    {
        switch (decompressionError)
        {
            case DecompressionError::CREATE_OUTPUT_PARENT_DIRECTORY:
                return "couldn't create output directory"sv;
            case DecompressionError::CREATE_OUTPUT_FILE:
                return "couldn't create output file"sv;
            case DecompressionError::WRITE_OUTPUT_FILE:
                return "output file write error"sv;
            case DecompressionError::OPEN_ARCHIVE_FILE:
                return "archive file open error"sv;
            case DecompressionError::READ_ARCHIVE_FILE:
                return "archive file read error"sv;
            case DecompressionError::NONE:
            default:
                return {};
        }
    }
}            // namespace mtasa
