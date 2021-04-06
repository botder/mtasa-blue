/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     XML-based global script settings storage
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string_view>
#include <filesystem>

namespace mtasa
{
    class ResourceManager;

    enum class SettingAccessType
    {
        PUBLIC,
        PROTECTED,
        PRIVATE,
    };

    class ScriptSettings final
    {
    public:
        static constexpr std::size_t MAX_NAME_LENGTH = 256;

        static constexpr char PREFIX_PUBLIC = '*';
        static constexpr char PREFIX_PROTECTED = '#';
        static constexpr char PREFIX_PRIVATE = '@';

        static constexpr char DELIMITER = '.';

    public:
        ScriptSettings(ResourceManager& resourceManager) : m_resourceManager{resourceManager}
        {
        }

        ~ScriptSettings();

        bool LoadSettings(const std::filesystem::path& filePath);

    private:
        ResourceManager&          m_resourceManager;
        std::unique_ptr<CXMLFile> m_document;
        CXMLNode*                 m_rootNode = nullptr;
    };
}
