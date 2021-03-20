/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource meta file parser
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "MetaFileParser.h"
#include <charconv>

using namespace std::string_literals;

namespace fs = std::filesystem;

namespace mtasa
{
    std::string MetaFileParser::Parse(const fs::path& filePath)
    {
        CXMLFile* rawDocument = g_pServerInterface->GetXML()->CreateXML(filePath.string().c_str());

        if (rawDocument == nullptr)
            return "failed to load XML document file"s;

        std::unique_ptr<CXMLFile> document{rawDocument};

        if (!document->Parse())
        {
            std::string errorText;
            document->GetLastError(errorText);
            return (errorText.empty() ? "failed to parse XML document file"s : errorText);
        }

        CXMLNode* root = document->GetRootNode();

        if (root == nullptr)
            return "XML document has no root node"s;

        using ProcessNodeFunction = void (MetaFileParser::*)(CXMLNode*);

        struct Processor
        {
            ProcessNodeFunction function = nullptr;
            bool                processOnlyOnce = false;
        };

        std::unordered_map<std::string, Processor> processors = {
            {"file"s, {&MetaFileParser::ProcessFileNode, false}},
            {"config"s, {&MetaFileParser::ProcessConfigNode, false}},
            {"script"s, {&MetaFileParser::ProcessScriptNode, false}},
            {"map"s, {&MetaFileParser::ProcessMapNode, false}},
            {"html"s, {&MetaFileParser::ProcessHtmlNode, false}},
            {"include"s, {&MetaFileParser::ProcessIncludeNode, false}},
            {"export"s, {&MetaFileParser::ProcessExportNode, false}},
            {"settings"s, {&MetaFileParser::ProcessSettingsNode, true}},
            {"info"s, {&MetaFileParser::ProcessInfoNode, true}},
            {"oop"s, {&MetaFileParser::ProcessOOPNode, true}},
            {"download_priority_group"s, {&MetaFileParser::ProcessDownloadPriorityGroupNode, true}},
            {"aclrequest"s, {&MetaFileParser::ProcessACLRequestNode, true}},
            {"sync_map_element_data"s, {&MetaFileParser::ProcessSyncMapElementDataNode, true}},
            {"min_mta_version"s, {&MetaFileParser::ProcessMinMTAVersionNode, true}},
        };
        
        m_isExportAlwaysACLRestricted = (m_resourceName == "webadmin"s || m_resourceName == "runcode"s);

        for (auto nodeIter = root->ChildrenBegin(); nodeIter != root->ChildrenEnd(); ++nodeIter)
        {
            CXMLNode*          node = *nodeIter;
            const std::string& nodeName = node->GetTagName();

            if (auto processorIter = processors.find(nodeName); processorIter != processors.end())
            {
                Processor& processor = processorIter->second;
                std::invoke(processor.function, this, node);

                if (processor.processOnlyOnce)
                    processors.erase(processorIter);
            }
        }

        return ""s;
    }

    void MetaFileParser::ProcessFileNode(CXMLNode* node)
    {
        MetaFileItem item;
        item.type = MetaFileItemType::FILE;
        item.isForClient = true;
        item.isForServer = false;

        CXMLAttributes& attributes = node->GetAttributes();
        bool            hasSourceFile = false;
        bool            hasDownload = false;

        for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
        {
            CXMLAttribute*     attribute = *iter;
            const std::string& name = attribute->GetName();
            const std::string& value = attribute->GetValue();

            if (name == "src"s)
            {
                if (!hasSourceFile)
                {
                    hasSourceFile = true;
                    item.sourceFile = fs::path{value, fs::path::generic_format}.lexically_normal();

                    if (item.sourceFile.empty())
                    {
                        CLogger::LogPrintf("WARNING: Empty 'src' attribute from 'file' node of 'meta.xml' for resource '%.*s', ignoring\n",
                                           m_resourceName.size(), m_resourceName.c_str());
                        return;
                    }
                }
            }
            else if (name == "download"s)
            {
                if (!hasDownload)
                {
                    hasDownload = true;

                    if (!stricmp(value.c_str(), "no") || !stricmp(value.c_str(), "false"))
                        item.isClientOptional = true;
                }
            }
        }

        if (!hasSourceFile)
        {
            CLogger::LogPrintf("WARNING: Missing 'src' attribute from 'file' node of 'meta.xml' for resource '%.*s', ignoring\n", m_resourceName.size(),
                               m_resourceName.c_str());
            return;
        }

        files.push_back(std::move(item));
    }

    void MetaFileParser::ProcessConfigNode(CXMLNode* node)
    {
        MetaFileItem item;
        item.type = MetaFileItemType::CONFIG;

        CXMLAttributes& attributes = node->GetAttributes();
        bool            hasSourceFile = false;
        bool            hasType = false;

        for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
        {
            CXMLAttribute*     attribute = *iter;
            const std::string& name = attribute->GetName();
            const std::string& value = attribute->GetValue();

            if (name == "src"s)
            {
                if (!hasSourceFile)
                {
                    hasSourceFile = true;
                    item.sourceFile = fs::path{value, fs::path::generic_format}.lexically_normal();

                    if (item.sourceFile.empty())
                    {
                        CLogger::LogPrintf("WARNING: Empty 'src' attribute from 'config' node of 'meta.xml' for resource '%.*s', ignoring\n",
                                           m_resourceName.size(), m_resourceName.c_str());
                        return;
                    }
                }
            }
            else if (name == "type"s)
            {
                if (!hasType)
                {
                    hasType = true;

                    if (!stricmp(value.c_str(), "shared"))
                    {
                        item.isForClient = true;
                    }
                    else if (!stricmp(value.c_str(), "client"))
                    {
                        item.isForServer = false;
                        item.isForClient = true;
                    }
                    else if (stricmp(value.c_str(), "server") != 0)
                    {
                        CLogger::LogPrintf("Unknown config type specified in %.*s. Assuming 'server'\n", m_resourceName.size(), m_resourceName.c_str());
                    }
                }
            }
        }

        if (!hasSourceFile)
        {
            CLogger::LogPrintf("WARNING: Missing 'src' attribute from 'config' node of 'meta.xml' for resource '%.*s', ignoring\n", m_resourceName.size(),
                               m_resourceName.c_str());
            return;
        }

        configs.push_back(std::move(item));
    }

    void MetaFileParser::ProcessScriptNode(CXMLNode* node)
    {
        MetaFileItem item;
        item.type = MetaFileItemType::SCRIPT;

        CXMLAttributes& attributes = node->GetAttributes();
        bool            hasSourceFile = false;
        bool            hasType = false;

        for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
        {
            CXMLAttribute*     attribute = *iter;
            const std::string& name = attribute->GetName();
            const std::string& value = attribute->GetValue();

            if (name == "src"s)
            {
                if (!hasSourceFile)
                {
                    hasSourceFile = true;
                    item.sourceFile = fs::path{value, fs::path::generic_format}.lexically_normal();

                    if (item.sourceFile.empty())
                    {
                        CLogger::LogPrintf("WARNING: Empty 'src' attribute from 'script' node of 'meta.xml' for resource '%.*s', ignoring\n",
                                           m_resourceName.size(), m_resourceName.c_str());
                        return;
                    }
                }
            }
            else if (name == "type"s)
            {
                if (!hasType)
                {
                    hasType = true;

                    if (!stricmp(value.c_str(), "shared"))
                    {
                        item.isForClient = true;
                    }
                    else if (!stricmp(value.c_str(), "client"))
                    {
                        item.isForServer = false;
                        item.isForClient = true;
                    }
                    else if (stricmp(value.c_str(), "server") != 0)
                    {
                        CLogger::LogPrintf("Unknown script type specified in %.*s. Assuming 'server'\n", m_resourceName.size(),
                                           m_resourceName.c_str());
                    }
                }
            }
            else if (name == "cache"s)
            {
                if (value == "false")
                {
                    item.isClientCacheable = false;
                }
            }
            else if (name == "protected")
            {
                if (value == "true")
                {
                    item.isClientCacheable = false;
                }
            }
        }

        if (!hasSourceFile)
        {
            CLogger::LogPrintf("WARNING: Missing 'src' attribute from 'script' node of 'meta.xml' for resource '%.*s', ignoring\n", m_resourceName.size(),
                               m_resourceName.c_str());
            return;
        }

        scripts.push_back(std::move(item));
    }

    void MetaFileParser::ProcessMapNode(CXMLNode* node)
    {
        MetaFileItem item;
        item.type = MetaFileItemType::MAP;

        CXMLAttributes& attributes = node->GetAttributes();
        bool            hasSourceFile = false;
        bool            hasDimension = false;

        for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
        {
            CXMLAttribute*     attribute = *iter;
            const std::string& name = attribute->GetName();
            const std::string& value = attribute->GetValue();

            if (name == "src"s)
            {
                if (!hasSourceFile)
                {
                    hasSourceFile = true;
                    item.sourceFile = fs::path{value, fs::path::generic_format}.lexically_normal();

                    if (item.sourceFile.empty())
                    {
                        CLogger::LogPrintf("WARNING: Empty 'src' attribute from 'map' node of 'meta.xml' for resource '%.*s', ignoring\n",
                                           m_resourceName.size(), m_resourceName.c_str());
                        return;
                    }
                }
            }
            else if (name == "dimension"s)
            {
                if (!hasDimension)
                {
                    hasDimension = true;

                    int dimension = 0;

                    if (auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), dimension); ec == std::errc{})
                    {
                        if (dimension >= 0 && dimension <= 65535)
                            item.dimension = static_cast<std::uint16_t>(dimension);
                    }
                }
            }
        }

        if (!hasSourceFile)
        {
            CLogger::LogPrintf("WARNING: Missing 'src' attribute from 'map' node of 'meta.xml' for resource '%.*s', ignoring\n", m_resourceName.size(),
                               m_resourceName.c_str());
            return;
        }

        maps.push_back(std::move(item));
    }

    void MetaFileParser::ProcessHtmlNode(CXMLNode* node)
    {
        MetaFileItem item;
        item.type = MetaFileItemType::HTML;

        CXMLAttributes& attributes = node->GetAttributes();
        bool            hasSourceFile = false;
        bool            hasDefault = false;
        bool            hasRaw = false;
        bool            hasRestricted = false;

        for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
        {
            CXMLAttribute*     attribute = *iter;
            const std::string& name = attribute->GetName();
            const std::string& value = attribute->GetValue();

            if (name == "src"s)
            {
                if (!hasSourceFile)
                {
                    hasSourceFile = true;
                    item.sourceFile = fs::path{value, fs::path::generic_format}.lexically_normal();

                    if (item.sourceFile.empty())
                    {
                        CLogger::LogPrintf("WARNING: Empty 'src' attribute from 'html' node of 'meta.xml' for resource '%.*s', ignoring\n",
                                           m_resourceName.size(), m_resourceName.c_str());
                        return;
                    }
                }
            }
            else if (name == "default"s)
            {
                if (!hasDefault)
                {
                    hasDefault = true;
                    item.isHttpDefault = (!stricmp(value.c_str(), "yes") || !stricmp(value.c_str(), "true"));
                }
            }
            else if (name == "raw"s)
            {
                if (!hasRaw)
                {
                    hasRaw = true;
                    item.isHttpRaw = (!stricmp(value.c_str(), "yes") || !stricmp(value.c_str(), "true"));
                }
            }
            else if (name == "restricted"s)
            {
                if (!hasRestricted)
                {
                    hasRestricted = true;
                    item.isHttpRestricted = (!stricmp(value.c_str(), "yes") || !stricmp(value.c_str(), "true"));
                }
            }
        }

        if (!hasSourceFile)
        {
            CLogger::LogPrintf("WARNING: Missing 'src' attribute from 'html' node of 'meta.xml' for resource '%.*s', ignoring\n", m_resourceName.size(),
                               m_resourceName.c_str());
            return;
        }

        htmls.push_back(std::move(item));
    }

    void MetaFileParser::ProcessIncludeNode(CXMLNode* node)
    {
        MetaDependencyItem item;

        CXMLAttributes& attributes = node->GetAttributes();
        bool            hasResource = false;
        bool            hasMinVersion = false;
        bool            hasMaxVersion = false;

        for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
        {
            CXMLAttribute*     attribute = *iter;
            const std::string& name = attribute->GetName();
            const std::string& value = attribute->GetValue();

            if (name == "resource"s)
            {
                if (!hasResource)
                {
                    hasResource = true;

                    if (value.empty())
                    {
                        CLogger::LogPrintf("WARNING: Empty 'resource' attribute from 'include' node of 'meta.xml' for resource '%.*s', ignoring\n",
                                           m_resourceName.size(), m_resourceName.c_str());
                        return;
                    }

                    item.resourceName = value;
                }
            }
            else if (name == "minversion"s)
            {
                if (!hasMinVersion)
                {
                    hasMinVersion = true;

                    std::stringstream ss{value};
                    ss >> item.minVersion.major;
                    ss >> item.minVersion.minor;
                    ss >> item.minVersion.revision;
                }
            }
            else if (name == "maxversion"s)
            {
                if (!hasMaxVersion)
                {
                    hasMaxVersion = true;

                    std::stringstream ss{value};
                    ss >> item.maxVersion.major;
                    ss >> item.maxVersion.minor;
                    ss >> item.maxVersion.revision;
                }
            }
        }

        if (!hasResource)
        {
            CLogger::LogPrintf("WARNING: Missing 'resource' attribute from 'include' node of 'meta.xml' for resource '%.*s', ignoring\n", m_resourceName.size(),
                               m_resourceName.c_str());
            return;
        }

        dependencies.push_back(std::move(item));
    }

    void MetaFileParser::ProcessSettingsNode(CXMLNode* node)
    {
        settingsNode = node->CopyNode(nullptr);
    }

    void MetaFileParser::ProcessInfoNode(CXMLNode* node)
    {
        CXMLAttributes& attributes = node->GetAttributes();

        for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
        {
            CXMLAttribute*     attribute = *iter;
            const std::string& name = attribute->GetName();
            const std::string& value = attribute->GetValue();

            info.try_emplace(name, value);

            if (name == "major"s)
            {
                unsigned int major = 0;

                if (auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), major); ec == std::errc{})
                {
                    version.major = major;
                }
            }
            else if (name == "minor"s)
            {
                unsigned int minor = 0;

                if (auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), minor); ec == std::errc{})
                {
                    version.minor = minor;
                }
            }
            else if (name == "revision"s)
            {
                unsigned int revision = 0;

                if (auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), revision); ec == std::errc{})
                {
                    version.revision = revision;
                }
            }
            else if (name == "state"s)
            {
                if (!stricmp(value.c_str(), "alpha"))
                {
                    versionStage = 0;
                }
                else if (!stricmp(value.c_str(), "beta"))
                {
                    versionStage = 1;
                }
                else
                {
                    versionStage = 2;
                }
            }
        }
    }

    void MetaFileParser::ProcessExportNode(CXMLNode* node)
    {
        MetaExportItem item;
        item.isACLRestricted = m_isExportAlwaysACLRestricted;

        CXMLAttributes& attributes = node->GetAttributes();
        bool            hasFunction = false;
        bool            hasType = false;
        bool            hasHttp = false;
        bool            hasRestricted = false;

        for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
        {
            CXMLAttribute*     attribute = *iter;
            const std::string& name = attribute->GetName();
            const std::string& value = attribute->GetValue();

            if (name == "function"s)
            {
                if (!hasFunction)
                {
                    hasFunction = true;

                    if (value.empty())
                    {
                        CLogger::LogPrintf("WARNING: Empty 'function' attribute from 'export' node of 'meta.xml' for resource '%.*s', ignoring\n",
                                           m_resourceName.size(), m_resourceName.c_str());
                        return;
                    }

                    item.functionName = value;
                }
            }
            else if (name == "type"s)
            {
                if (!hasType)
                {
                    hasType = true;

                    if (!stricmp(value.c_str(), "shared"))
                    {
                        item.isForClient = true;
                    }
                    else if (!stricmp(value.c_str(), "client"))
                    {
                        item.isForServer = false;
                        item.isForClient = true;
                    }
                    else if (stricmp(value.c_str(), "server") != 0)
                    {
                        CLogger::LogPrintf("Unknown exported function type specified in %.*s. Assuming 'server'\n", m_resourceName.size(), m_resourceName.c_str());
                    }
                }
            }
            else if (name == "http"s)
            {
                if (!hasHttp)
                {
                    hasHttp = true;
                    item.isHttpAccessible = (!stricmp(value.c_str(), "yes") || !stricmp(value.c_str(), "true"));
                }
            }
            else if (name == "restricted")
            {
                if (!hasRestricted)
                {
                    hasRestricted = true;
                    item.isACLRestricted = (m_isExportAlwaysACLRestricted || !stricmp(value.c_str(), "yes") || !stricmp(value.c_str(), "true"));
                }
            }
        }

        if (!hasFunction)
        {
            CLogger::LogPrintf("WARNING: Missing 'function' attribute from 'export' node of 'meta.xml' for resource '%.*s', ignoring\n", m_resourceName.size(),
                               m_resourceName.c_str());
            return;
        }

        exports.push_back(std::move(item));
    }

    void MetaFileParser::ProcessOOPNode(CXMLNode* node)
    {
        const std::string& value = node->GetTagContent();
        useOOP = (value == "true" || value == "1" || value == "yes");
    }

    void MetaFileParser::ProcessDownloadPriorityGroupNode(CXMLNode* node)
    {
        const std::string& value = node->GetTagContent();
        int                group = 0;

        if (auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), group); ec == std::errc{})
        {
            downloadPriorityGroup = group;
        }
    }

    void MetaFileParser::ProcessACLRequestNode(CXMLNode* node)
    {
        for (auto iter = node->ChildrenBegin(); iter != node->ChildrenEnd(); ++iter)
        {
            CXMLNode*          rightNode = *iter;
            const std::string& nodeName = rightNode->GetTagName();

            if (nodeName == "right"s)
            {
                SString rawName = rightNode->GetAttributeValue("name");
                SString rawAccess = rightNode->GetAttributeValue("access");
                bool    accessRequired = (rawAccess == "true"s || rawAccess == "1"s || rawAccess == "yes"s);

                if (!rawName.empty())
                {
                    aclRequests.try_emplace(std::move(rawName), accessRequired);
                }
            }
        }
    }

    void MetaFileParser::ProcessSyncMapElementDataNode(CXMLNode* node)
    {
        const std::string& value = node->GetTagContent();
        syncMapElementData = (value == "true" || value == "1" || value == "yes");
        syncMapElementDataDefined = true;
    }

    void MetaFileParser::ProcessMinMTAVersionNode(CXMLNode* node)
    {
        CXMLAttributes& attributes = node->GetAttributes();

        if (CXMLAttribute* attribute = attributes.Find("both"); attribute != nullptr)
        {
            minClientVersion = attribute->GetValue();
            minServerVersion = attribute->GetValue();
        }
        else
        {
            if (attribute = attributes.Find("server"); attribute != nullptr)
            {
                minServerVersion = attribute->GetValue();
            }

            if (attribute = attributes.Find("client"); attribute != nullptr)
            {
                minClientVersion = attribute->GetValue();
            }
        }
    }
}
