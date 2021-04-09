/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource file content checker/validator/upgrader
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ResourceChecker.h"
#include "Resource.h"
#include "ResourceFile.h"
#include <array>

namespace mtasa
{
    extern bool IsRenderWareFileHierarchyCorrupt(const std::filesystem::path& filePath);

    void ResourceChecker::RunAnalysis()
    {
        // Check meta.xml
        AnalyseMetaFile(m_resource.GetSourceDirectory() / "meta.xml"sv);

        // Check resource files
        for (const std::unique_ptr<ResourceFile>& resourceFile : m_resource)
        {
            if (!resourceFile->IsValidatable())
                continue;

            switch (resourceFile->GetType())
            {
                case ResourceFileType::CLIENT_FILE:
                    AnalyseRegularFile(*resourceFile);
                    break;
                case ResourceFileType::CLIENT_SCRIPT:
                    AnalyseLuaFile(*resourceFile, true);
                    break;
                case ResourceFileType::SERVER_SCRIPT:
                    AnalyseLuaFile(*resourceFile, false);
                    break;
                default:
                    break;
            }
        }
    }

    void ResourceChecker::AnalyseMetaFile(const std::filesystem::path& filePath)
    {
        // TODO: Add implementation here
    }

    void ResourceChecker::AnalyseLuaFile(const ResourceFile& resourceFile, bool isForClient)
    {
        // TODO: Add implementation here
    }

    void ResourceChecker::AnalyseRegularFile(const ResourceFile& resourceFile)
    {
        std::string extension = resourceFile.GetRelativePath().extension().generic_string();

        if (extension.empty())
            return;

        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return tolower(c); });

        if (extension == ".png"sv || extension == ".jpg"sv)
        {
            AnalyseImageFile(resourceFile);
        }
        else if (extension == ".txd"sv || extension == ".dff"sv)
        {
            if (IsRenderWareFileHierarchyCorrupt(resourceFile.GetSourceFilePath()))
            {
                // TODO: LOG: WARNING: File '<fileName>' in resource '<resourceName>' contains errors.\n
            }
        }
    }

    void ResourceChecker::AnalyseImageFile(const ResourceFile& resourceFile)
    {
        std::unique_ptr<std::FILE, decltype(&fclose)> stream{File::Fopen(resourceFile.GetSourceFilePath().string().c_str(), "rb"), &fclose};

        if (stream == nullptr)
            return;

        std::uint64_t header = 0;

        if (fread(&header, sizeof(header), 1, stream.get()) == 1)
        {
            static constexpr std::uint64_t PNG_HEADER{0x0A'1A'0A'0D'47'4E'50'89};
            static constexpr std::uint64_t JPG_HEADER{0x00'00'00'00'00'FF'D8'FF};

            if (header != PNG_HEADER && (header & JPG_HEADER) != JPG_HEADER)
            {
                // TODO: LOG: WARNING: File '<fileName>' in resource '<resourceName>' is invalid.\n
            }
        }
    }
}            // namespace mtasa
