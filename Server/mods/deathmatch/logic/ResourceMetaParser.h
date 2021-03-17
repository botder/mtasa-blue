/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource meta file parsing
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

class CXMLNode;

namespace mtasa
{
    class ResourceMetaParser
    {
    public:
        ResourceMetaParser(std::string_view resourceName);

        std::optional<std::string> Parse(const std::filesystem::path& metaFile);

    private:
        void LogWarning(const std::string& warning);
        void LogWarning(std::string&& warning) { LogWarning(warning); }

        void LogError(const std::string& error) { m_errors.push_back(error); }
        void LogError(std::string&& error) { m_errors.push_back(std::move(error)); }

        bool ParseSettingsNode(CXMLNode* node);
        bool ParseMinVersionRequirementNode(CXMLNode* node);
        bool ParseACLRequestsNode(CXMLNode* node);
        bool ParseElementDataSyncNode(CXMLNode* node);
        bool ParseOOPNode(CXMLNode* node);
        bool ParseDownloadPriorityGroupNode(CXMLNode* node);
        bool ParseInfoNode(CXMLNode* node);
        bool ParseResourceDependencyNode(CXMLNode* node);
        bool ParseMapNode(CXMLNode* node);
        bool ParseFileNode(CXMLNode* node);
        bool ParseScriptNode(CXMLNode* node);
        bool ParseHtmlNode(CXMLNode* node);
        bool ParseFunctionExportNode(CXMLNode* node);
        bool ParseConfigFileNode(CXMLNode* node);

    private:
        std::string m_resourceName;

        std::vector<std::string> m_errors;

        using NodeParsingFunction = bool (ResourceMetaParser::*)(CXMLNode*);
        std::unordered_map<std::string, NodeParsingFunction> m_nodeParsers;
    };
}
