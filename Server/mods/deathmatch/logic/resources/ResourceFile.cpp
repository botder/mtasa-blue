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
    bool ResourceFile::CalculateFileMetaData()
    {
        fs::path        filePath{m_resource.GetSourceDirectory() / m_relativePath};
        std::error_code errorCode;

        m_size = fs::file_size(filePath, errorCode);
        m_exists = (errorCode.value() == 0);

        if (!m_exists)
        {
            m_size = 0;
            m_checksum.Reset();
            return false;
        }

        return m_checksum.Calculate(filePath);
    }
}            // namespace mtasa
