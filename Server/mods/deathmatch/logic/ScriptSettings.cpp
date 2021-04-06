/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     XML-based global script settings storage
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ScriptSettings.h"

namespace mtasa
{
    ScriptSettings::~ScriptSettings() = default;

    bool ScriptSettings::LoadSettings(const std::filesystem::path& filePath)
    {
        std::unique_ptr<CXMLFile> document{g_pServerInterface->GetXML()->CreateXML(filePath.string().c_str())};

        if (document == nullptr)
        {
            CLogger::ErrorPrintf("Could not create XML instance for settings registry! Registry disabled.\n");
            return false;
        }

        if (!document->Parse())
        {
            std::string errorText;
            CXMLErrorCodes::Code errorCode = document->GetLastError(errorText);

            if (errorCode != CXMLErrorCodes::Code::NoFileSpecified)
            {
                CLogger::ErrorPrintf("Parsing global script settings file failed: %s\n", errorText.size(), errorText.c_str());
                return false;
            }
        }

        if (m_rootNode = document->GetRootNode(); m_rootNode == nullptr)
        {
            m_rootNode = document->CreateRootNode("settings");

            if (!document->Write())
            {
                CLogger::ErrorPrintf("Error saving global script settings file\n");
                m_rootNode = nullptr;
                return false;
            }
        }

        m_document = std::move(document);
        return true;
    }
}            // namespace mtasa
