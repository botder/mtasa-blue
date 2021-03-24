/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource meta file parser
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <vector>
#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

namespace mtasa
{
    enum class MetaFileItemType
    {
        UNKNOWN,
        FILE,
        CONFIG,
        SCRIPT,
        MAP,
        HTML,
    };

    class MetaFileVersion final
    {
    public:
        std::uint32_t major = 0;
        std::uint32_t minor = 0;
        std::uint32_t revision = 0;
    };

    class MetaFileItem final
    {
    public:
        MetaFileItemType      type = MetaFileItemType::UNKNOWN;
        std::filesystem::path sourceFile;
        std::uint16_t         dimension = 0;
        std::uint8_t          isForClient : 1;
        std::uint8_t          isForServer : 1;
        std::uint8_t          isClientOptional : 1;
        std::uint8_t          isClientCacheable : 1;
        std::uint8_t          isHttpDefault : 1;
        std::uint8_t          isHttpRestricted : 1;
        std::uint8_t          isHttpRaw : 1;

        MetaFileItem()
            : isForClient{false},
              isForServer{true},
              isClientOptional{false},
              isClientCacheable{true},
              isHttpDefault{false},
              isHttpRestricted{false},
              isHttpRaw{false}
        {
        }
    };

    class MetaDependencyItem final
    {
    public:
        std::string     resourceName;
        MetaFileVersion minVersion;
        MetaFileVersion maxVersion;
    };

    class MetaExportItem final
    {
    public:
        std::string   functionName;
        std::uint8_t  isForClient : 1;
        std::uint8_t  isForServer : 1;
        std::uint8_t  isHttpAccessible : 1;
        std::uint8_t  isACLRestricted : 1;

        MetaExportItem() : isForClient{false}, isForServer{true}, isHttpAccessible(false), isACLRestricted(false) {}
    };

    class MetaFileParser final
    {
    public:
        MetaFileParser(std::string_view resourceName) : m_resourceName{resourceName}, useOOP{false}, syncMapElementData{true}, syncMapElementDataDefined{false}
        {
        }

        std::string Parse(const std::filesystem::path& filePath);

    private:
        void ProcessFileNode(CXMLNode* node);
        void ProcessConfigNode(CXMLNode* node);
        void ProcessScriptNode(CXMLNode* node);
        void ProcessMapNode(CXMLNode* node);
        void ProcessHtmlNode(CXMLNode* node);
        void ProcessIncludeNode(CXMLNode* node);
        void ProcessSettingsNode(CXMLNode* node);
        void ProcessInfoNode(CXMLNode* node);
        void ProcessExportNode(CXMLNode* node);
        void ProcessOOPNode(CXMLNode* node);
        void ProcessDownloadPriorityGroupNode(CXMLNode* node);
        void ProcessACLRequestNode(CXMLNode* node);
        void ProcessSyncMapElementDataNode(CXMLNode* node);
        void ProcessMinMTAVersionNode(CXMLNode* node);

    private:
        std::string m_resourceName;
        bool        m_isExportAlwaysACLRestricted = false;

    public:
        std::unordered_map<std::string, std::string> info;

        std::string minClientVersion;
        std::string minServerVersion;

        std::uint8_t useOOP : 1;
        std::uint8_t syncMapElementData : 1;
        std::uint8_t syncMapElementDataDefined : 1;

        int downloadPriorityGroup = 0;

        std::vector<MetaFileItem>       files;
        std::vector<MetaFileItem>       configs;
        std::vector<MetaFileItem>       scripts;
        std::vector<MetaFileItem>       maps;
        std::vector<MetaFileItem>       htmls;
        std::vector<MetaDependencyItem> dependencies;
        std::vector<MetaExportItem>     exports;

        std::unordered_map<std::string, bool> aclRequests;

        CXMLNode* settingsNode = nullptr;
    };
}
