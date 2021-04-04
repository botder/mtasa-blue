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

namespace fs = std::filesystem;

namespace mtasa
{
    bool ResourceFile::CalculateFileMetaData()
    {
        constexpr std::size_t GIBIBYTE = 1 * 1024 * 1024 * 1024;

        std::string fileContent;

        if (!ReadEntireFile(m_resource.GetSourceDirectory() / m_relativePath, fileContent, GIBIBYTE))
            return false;

        m_checksum = CChecksum::GenerateChecksumFromBuffer(fileContent.data(), fileContent.size());
        m_size = fileContent.size();
        return true;
    }
}            // namespace mtasa
