/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     A server-side script resource file
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ResourceScriptFile.h"
#include "Resource.h"

namespace mtasa
{
    bool ResourceScriptFile::Start()
    {
        if (m_isRunning)
            return false;

        constexpr std::size_t GIBIBYTE = 1 * 1024 * 1024 * 1024;

        std::string fileContent;

        if (!ReadEntireFile(m_resource.GetSourceDirectory() / m_relativePath, fileContent, GIBIBYTE))
            return false;

        if (!fileContent.empty())
        {
            m_resource.GetLuaContext()->LoadScriptFromBuffer(fileContent.data(), fileContent.size(), m_name.c_str());
        }

        m_isRunning = true;
        return true;
    }
}            // namespace mtasa
