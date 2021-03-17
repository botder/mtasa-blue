/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource meta file parsing
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ResourceMetaParser.h"
#include <memory>
#include <charconv>

using namespace std::string_literals;

mtasa::ResourceMetaParser::ResourceMetaParser(std::string_view resourceName) : m_resourceName{resourceName}
{
    m_nodeParsers.try_emplace("settings"s, &ResourceMetaParser::ParseSettingsNode);
    m_nodeParsers.try_emplace("min_mta_version"s, &ResourceMetaParser::ParseMinVersionRequirementNode);
    m_nodeParsers.try_emplace("aclrequest"s, &ResourceMetaParser::ParseACLRequestsNode);
    m_nodeParsers.try_emplace("sync_map_element_data"s, &ResourceMetaParser::ParseElementDataSyncNode);
    m_nodeParsers.try_emplace("oop"s, &ResourceMetaParser::ParseOOPNode);
    m_nodeParsers.try_emplace("download_priority_group"s, &ResourceMetaParser::ParseDownloadPriorityGroupNode);
    m_nodeParsers.try_emplace("info"s, &ResourceMetaParser::ParseInfoNode);
    m_nodeParsers.try_emplace("include"s, &ResourceMetaParser::ParseResourceDependencyNode);
    m_nodeParsers.try_emplace("map"s, &ResourceMetaParser::ParseMapNode);
    m_nodeParsers.try_emplace("file"s, &ResourceMetaParser::ParseFileNode);
    m_nodeParsers.try_emplace("script"s, &ResourceMetaParser::ParseScriptNode);
    m_nodeParsers.try_emplace("html"s, &ResourceMetaParser::ParseHtmlNode);
    m_nodeParsers.try_emplace("export"s, &ResourceMetaParser::ParseFunctionExportNode);
    m_nodeParsers.try_emplace("config"s, &ResourceMetaParser::ParseConfigFileNode);
}

std::optional<std::string> mtasa::ResourceMetaParser::Parse(const std::filesystem::path& metaFile)
{
    CXMLFile* rawDocument = g_pServerInterface->GetXML()->CreateXML(metaFile.string().c_str());

    if (rawDocument == nullptr)
        return "failed to load XML document"s;

    std::unique_ptr<CXMLFile> document{rawDocument};

    if (!document->Parse())
    {
        std::string lastError;
        document->GetLastError(lastError);
        return lastError;
    }

    CXMLNode* root = document->GetRootNode();

    if (root == nullptr)
        return "XML document contains no root node"s;

    for (auto nodeIter = root->ChildrenBegin(); nodeIter != root->ChildrenEnd(); ++nodeIter)
    {
        CXMLNode* node = *nodeIter;

        if (auto parserIter = m_nodeParsers.find(node->GetTagName()); parserIter != m_nodeParsers.end())
        {
            NodeParsingFunction parserFunc = parserIter->second;
            std::invoke(parserFunc, this, node);
        }
    }

    if (m_errors.empty())
    {
        return std::nullopt;
    }
    else if (m_errors.size() == 1)
    {
        return m_errors[0];
    }
    else
    {
        std::ostringstream errorStream;
        std::copy(m_errors.begin(), m_errors.end(), std::ostream_iterator<std::string>{errorStream, ", "});
        return errorStream.str();
    }
}

void mtasa::ResourceMetaParser::LogWarning(const std::string& warning)
{
    CLogger::LogPrintf("WARNING: %.*s for resource '%.*s'\n", warning.size(), warning.c_str(), m_resourceName.size(), m_resourceName.c_str());
}

bool mtasa::ResourceMetaParser::ParseSettingsNode(CXMLNode* node)
{
    // CResource::m_pNodeSettings = node->CopyNode(nullptr);
    return true;
}

bool mtasa::ResourceMetaParser::ParseMinVersionRequirementNode(CXMLNode* node)
{
    CXMLAttributes& attributes = node->GetAttributes();

    if (CXMLAttribute* attribute = attributes.Find("both"); attribute != nullptr)
    {
        // CResource::m_strMinServerFromMetaXml = CResource::m_strMinClientFromMetaXml = attribute->GetValue();
    }
    else
    {
        if (attribute = attributes.Find("server"); attribute != nullptr)
        {
            // CResource::m_strMinServerFromMetaXml = pAttr->GetValue();
        }

        if (attribute = attributes.Find("client"); attribute != nullptr)
        {
            // CResource::m_strMinClientFromMetaXml = pAttr->GetValue();
        }
    }

    return true;
}

bool mtasa::ResourceMetaParser::ParseACLRequestsNode(CXMLNode* node)
{
    // CResource::RefreshAutoPermissions(pNodeAclRequest);
    return true;
}

bool mtasa::ResourceMetaParser::ParseElementDataSyncNode(CXMLNode* node)
{
    const std::string& value = node->GetTagContent();
    bool enabled = (value == "true" || value == "1" || value == "yes");
    // CResource::m_bSyncMapElementData = enabled
    // CResource::m_bSyncMapElementDataDefined = true
    return true;
}

bool mtasa::ResourceMetaParser::ParseOOPNode(CXMLNode* node)
{
    const std::string& value = node->GetTagContent();
    bool enabled = (value == "true" || value == "1" || value == "yes");
    // CResource::m_bOOPEnabledInMetaXml = enabled
    return true;
}

bool mtasa::ResourceMetaParser::ParseDownloadPriorityGroupNode(CXMLNode* node)
{
    const std::string& value = node->GetTagContent();
    int group = 0;

    auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), group);

    if (ec == std::errc{})
    {
        // CResource::m_iDownloadPriorityGroup = group
    }
    else
    {
        LogWarning(SString("Could not convert download priority group '%.*s' to a signed integer", value.size(), value.c_str()));
    }

    return true;
}

bool mtasa::ResourceMetaParser::ParseInfoNode(CXMLNode* node)
{
    CXMLAttributes& attributes = node->GetAttributes();

    for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
    {
        CXMLAttribute* attribute = *iter;
        const std::string& name = attribute->GetName();
        const std::string& value = attribute->GetValue();
        // MapSet(CResource::m_Info, name, value);

        if (name == "major"s)
        {
            unsigned int major = 0;
            auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), major);

            if (ec == std::errc{})
            {
                // CResource::m_uiVersionMajor = major
            }
            else
            {
                LogWarning(SString("Could not convert resource version major '%.*s' to an unsigned integer", value.size(), value.c_str()));
            }
        }
        else if (name == "minor"s)
        {
            unsigned int minor = 0;
            auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), minor);

            if (ec == std::errc{})
            {
                // CResource::m_uiVersionMinor = minor
            }
            else
            {
                LogWarning(SString("Could not convert resource version minor '%.*s' to an unsigned integer", value.size(), value.c_str()));
            }
        }
        else if (name == "revision"s)
        {
            unsigned int revision = 0;
            auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), revision);

            if (ec == std::errc{})
            {
                // CResource::m_uiVersionRevision = revision
            }
            else
            {
                LogWarning(SString("Could not convert resource version revision '%.*s' to an unsigned integer", value.size(), value.c_str()));
            }
        }
        else if (name == "state"s)
        {
            if (!stricmp(value.c_str(), "alpha"))
            {
                // CResource::m_uiVersionState = 0
            }
            else if (!stricmp(value.c_str(), "beta"))
            {
                // CResource::m_uiVersionState = 1
            }
            else
            {
                // CResource::m_uiVersionState = 2
            }
        }
    }

    return true;
}

bool mtasa::ResourceMetaParser::ParseResourceDependencyNode(CXMLNode* node)
{
    CXMLAttributes& attributes = node->GetAttributes();

    CXMLAttribute* minVersionAttr = nullptr;
    CXMLAttribute* maxVersionAttr = nullptr;
    CXMLAttribute* resourceAttr = nullptr;

    for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
    {
        CXMLAttribute*     attribute = *iter;
        const std::string& name = attribute->GetName();

        if (name == "resource"s)
        {
            resourceAttr = attribute;
        }
        else if (name == "minversion"s)
        {
            minVersionAttr = attribute;
        }
        else if (name == "maxversion"s)
        {
            maxVersionAttr = attribute;
        }
    }

    if (resourceAttr == nullptr)
    {
        LogWarning("Missing 'resource' attribute from 'include' node of 'meta.xml'"s);
        return false;
    }
    else if (resourceAttr->GetValue().empty())
    {
        LogWarning("Empty 'resource' attribute from 'include' node of 'meta.xml'"s);
        return false;
    }

    SVersion minVersion;

    if (minVersionAttr != nullptr && !minVersionAttr->GetValue().empty())
    {
        std::stringstream ss{minVersionAttr->GetValue()};
        ss >> minVersion.m_uiMajor;
        ss >> minVersion.m_uiMinor;
        ss >> minVersion.m_uiRevision;
    }

    SVersion maxVersion;

    if (maxVersionAttr != nullptr && !maxVersionAttr->GetValue().empty())
    {
        std::stringstream ss{maxVersionAttr->GetValue()};
        ss >> maxVersion.m_uiMajor;
        ss >> maxVersion.m_uiMinor;
        ss >> maxVersion.m_uiRevision;
    }

    const std::string& resourceName = resourceAttr->GetValue();
    // CResource::m_IncludedResources.push_back(new CIncludedResources(m_pResourceManager, resourceName.c_str(), minVersion, maxVersion, this));
    return true;
}

bool mtasa::ResourceMetaParser::ParseMapNode(CXMLNode* node)
{
    CXMLAttributes& attributes = node->GetAttributes();

    CXMLAttribute* srcAttr = nullptr;
    CXMLAttribute* dimensionAttr = nullptr;

    for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
    {
        CXMLAttribute*     attribute = *iter;
        const std::string& name = attribute->GetName();

        if (name == "src"s)
        {
            srcAttr = attribute;
        }
        else if (name == "dimension"s)
        {
            dimensionAttr = attribute;
        }
    }

    if (srcAttr == nullptr)
    {
        LogWarning("Missing 'src' attribute from 'map' node of 'meta.xml'"s);
        return false;
    }
    else if (srcAttr->GetValue().empty())
    {
        LogWarning("Empty 'src' attribute from 'map' node of 'meta.xml'"s);
        return false;
    }

    // TODO:
    // ReplaceSlashes(strFilename);
    // IsValidFilePath(strFilename.c_str())
    // GetFilePath(strFilename.c_str(), strFullFilename)

    // TODO: Duplicate path check
    // "Duplicate map file in resource '%s': '%s'\n"

    int dimension = 0;

    if (dimensionAttr != nullptr && !dimensionAttr->GetValue().empty())
    {
        const std::string& value = dimensionAttr->GetValue();
        auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), dimension);

        if (ec == std::errc{})
        {
            if (dimension < 0 || dimension > 65535)
            {
                dimension = 0;
            }
        }
    }

    // CResource::m_ResourceFiles.push_back(new CResourceMapItem(this, strFilename.c_str(), strFullFilename.c_str(), &Attributes, iDimension));
    return true;
}

bool mtasa::ResourceMetaParser::ParseFileNode(CXMLNode* node)
{
    CXMLAttributes& attributes = node->GetAttributes();

    CXMLAttribute* srcAttr = nullptr;
    CXMLAttribute* downloadAttr = nullptr;

    for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
    {
        CXMLAttribute*     attribute = *iter;
        const std::string& name = attribute->GetName();

        if (name == "src"s)
        {
            srcAttr = attribute;
        }
        else if (name == "download"s)
        {
            downloadAttr = attribute;
        }
    }

    if (srcAttr == nullptr)
    {
        LogWarning("Missing 'src' attribute from 'file' node of 'meta.xml'"s);
        return false;
    }
    else if (srcAttr->GetValue().empty())
    {
        LogWarning("Empty 'src' attribute from 'file' node of 'meta.xml'"s);
        return false;
    }

    // TODO:
    // ReplaceSlashes(strFilename);
    // IsValidFilePath(strFilename.c_str())
    // GetFilePath(strFilename.c_str(), strFullFilename)

    // TODO: Duplicate path check
    // "Duplicate map file in resource '%s': '%s'\n"

    bool download = true;

    if (downloadAttr != nullptr && !downloadAttr->GetValue().empty())
    {
        const std::string& value = downloadAttr->GetValue();

        if (!stricmp(value.c_str(), "no") || !stricmp(value.c_str(), "false"))
            download = false;
    }

    // CResource::m_ResourceFiles.push_back(new CResourceClientFileItem(this, strFilename.c_str(), strFullFilename.c_str(), &Attributes, download));
    return true;
}

bool mtasa::ResourceMetaParser::ParseScriptNode(CXMLNode* node)
{
    CXMLAttributes& attributes = node->GetAttributes();

    CXMLAttribute* srcAttr = nullptr;
    CXMLAttribute* typeAttr = nullptr;
    CXMLAttribute* cacheAttr = nullptr;
    CXMLAttribute* protectedAttr = nullptr;

    for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
    {
        CXMLAttribute*     attribute = *iter;
        const std::string& name = attribute->GetName();

        if (name == "src"s)
        {
            srcAttr = attribute;
        }
        else if (name == "type"s)
        {
            typeAttr = attribute;
        }
        else if (name == "cache"s)
        {
            cacheAttr = attribute;
        }
        else if (name == "protected"s)
        {
            protectedAttr = attribute;
        }
    }

    if (srcAttr == nullptr)
    {
        LogWarning("Missing 'src' attribute from 'script' node of 'meta.xml'"s);
        return false;
    }
    else if (srcAttr->GetValue().empty())
    {
        LogWarning("Empty 'src' attribute from 'script' node of 'meta.xml'"s);
        return false;
    }

    // TODO:
    // ReplaceSlashes(strFilename);
    // IsValidFilePath(strFilename.c_str())
    // GetFilePath(strFilename.c_str(), strFullFilename)

    // TODO: Duplicate path check
    // "Duplicate script file in resource '%s': '%s'\n"

    int type = 0; // 0 = server, 1 = client, 2 = both

    if (typeAttr != nullptr)
    {
        const std::string& value = typeAttr->GetValue();

        if (!stricmp(value.c_str(), "shared"))
        {
            type = 2;
        }
        else if (!stricmp(value.c_str(), "client"))
        {
            type = 1;
        }
        else if (!stricmp(value.c_str(), "server"))
        {
            type = 0;
        }
        else
        {
            LogWarning(SString("Unknown 'type' attribute value '%.*s' (defaults to: server) from 'script' node of 'meta.xml'", value.size(), value.c_str()));
        }
    }

    bool disableClientCache = false;

    if (protectedAttr != nullptr && protectedAttr->GetValue() == "true")
        disableClientCache = true;
    else if (cacheAttr != nullptr && cacheAttr->GetValue() == "false")
        disableClientCache = true;

    // if (bServer)
    //     CResource::m_ResourceFiles.push_back(new CResourceScriptItem(this, strFilename.c_str(), strFullFilename.c_str(), &Attributes));
    // if (bClient)
    //     CResource::m_ResourceFiles.push_back(new CResourceClientScriptItem(this, strFilename.c_str(), strFullFilename.c_str(), &Attributes));
    return true;
}

bool mtasa::ResourceMetaParser::ParseHtmlNode(CXMLNode* node)
{
    CXMLAttributes& attributes = node->GetAttributes();

    CXMLAttribute* srcAttr = nullptr;
    CXMLAttribute* defaultAttr = nullptr;
    CXMLAttribute* rawAttr = nullptr;
    CXMLAttribute* restrictedAttr = nullptr;

    for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
    {
        CXMLAttribute*     attribute = *iter;
        const std::string& name = attribute->GetName();

        if (name == "src"s)
        {
            srcAttr = attribute;
        }
        else if (name == "default"s)
        {
            defaultAttr = attribute;
        }
        else if (name == "raw"s)
        {
            rawAttr = attribute;
        }
        else if (name == "restricted"s)
        {
            restrictedAttr = attribute;
        }
    }

    if (srcAttr == nullptr)
    {
        LogWarning("Missing 'src' attribute from 'html' node of 'meta.xml'"s);
        return false;
    }
    else if (srcAttr->GetValue().empty())
    {
        LogWarning("Empty 'src' attribute from 'html' node of 'meta.xml'"s);
        return false;
    }

    // TODO:
    // ReplaceSlashes(strFilename);
    // IsValidFilePath(strFilename.c_str())
    // GetFilePath(strFilename.c_str(), strFullFilename)

    // TODO: Duplicate path check
    // "Duplicate html file in resource '%s': '%s'\n"

    bool isDefault = false;

    if (defaultAttr != nullptr && !defaultAttr->GetValue().empty())
    {
        const std::string& value = defaultAttr->GetValue();

        if (!stricmp(value.c_str(), "yes") || !stricmp(value.c_str(), "true"))
            isDefault = true;
    }

    bool isRaw = false;

    if (rawAttr != nullptr && !rawAttr->GetValue().empty())
    {
        const std::string& value = rawAttr->GetValue();

        if (!stricmp(value.c_str(), "yes") || !stricmp(value.c_str(), "true"))
            isRaw = true;
    }

    bool isRestricted = false;

    if (restrictedAttr != nullptr && !restrictedAttr->GetValue().empty())
    {
        const std::string& value = restrictedAttr->GetValue();

        if (!stricmp(value.c_str(), "yes") || !stricmp(value.c_str(), "true"))
            isRestricted = true;
    }

    // auto pResourceFile =
    //     new CResourceHTMLItem(this, strFilename.c_str(), strFullFilename.c_str(), &Attributes, bIsDefault, bIsRaw, bIsRestricted, m_bOOPEnabledInMetaXml);
    // CResource::m_ResourceFiles.push_back(pResourceFile);
    return true;
}

bool mtasa::ResourceMetaParser::ParseFunctionExportNode(CXMLNode* node)
{
    CXMLAttributes& attributes = node->GetAttributes();

    CXMLAttribute* functionAttr = nullptr;
    CXMLAttribute* typeAttr = nullptr;
    CXMLAttribute* httpAttr = nullptr;
    CXMLAttribute* restrictedAttr = nullptr;

    for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
    {
        CXMLAttribute*     attribute = *iter;
        const std::string& name = attribute->GetName();

        if (name == "function"s)
        {
            functionAttr = attribute;
        }
        else if (name == "type"s)
        {
            typeAttr = attribute;
        }
        else if (name == "http"s)
        {
            httpAttr = attribute;
        }
        else if (name == "restricted"s)
        {
            restrictedAttr = attribute;
        }
    }

    if (functionAttr == nullptr)
    {
        LogWarning("Missing 'function' attribute from 'export' node of 'meta.xml'"s);
        return false;
    }
    else if (functionAttr->GetValue().empty())
    {
        LogWarning("Empty 'function' attribute from 'export' node of 'meta.xml'"s);
        return false;
    }

    // TODO:
    // ReplaceSlashes(strFilename);
    // IsValidFilePath(strFilename.c_str())
    // GetFilePath(strFilename.c_str(), strFullFilename)

    int type = 0;            // 0 = server, 1 = client, 2 = both

    if (typeAttr != nullptr)
    {
        const std::string& value = typeAttr->GetValue();

        if (!stricmp(value.c_str(), "shared"))
        {
            type = 2;
        }
        else if (!stricmp(value.c_str(), "client"))
        {
            type = 1;
        }
        else if (!stricmp(value.c_str(), "server"))
        {
            type = 0;
        }
        else
        {
            LogWarning(SString("Unknown 'type' attribute value '%.*s' (defaults to: server) from 'export' node of 'meta.xml'", value.size(), value.c_str()));
        }
    }

    bool isHttp = false;

    if (httpAttr != nullptr && !httpAttr->GetValue().empty())
    {
        const std::string& value = httpAttr->GetValue();

        if (!stricmp(value.c_str(), "yes") || !stricmp(value.c_str(), "true"))
            isHttp = true;
    }

    bool isRestricted = false;

    if (restrictedAttr != nullptr && !restrictedAttr->GetValue().empty())
    {
        const std::string& value = restrictedAttr->GetValue();

        if (!stricmp(value.c_str(), "yes") || !stricmp(value.c_str(), "true"))
            isRestricted = true;
    }

    if (!isRestricted)
    {
        isRestricted = (m_resourceName == "webadmin"s || m_resourceName == "runcode"s);
    }

    // if (bServer)
    //     m_ExportedFunctions.push_back(CExportedFunction(strFunction.c_str(), bHTTP, CExportedFunction::EXPORTED_FUNCTION_TYPE_SERVER,
    //                                                     isRestricted);
    // if (bClient)
    //     m_ExportedFunctions.push_back(CExportedFunction(strFunction.c_str(), bHTTP, CExportedFunction::EXPORTED_FUNCTION_TYPE_CLIENT,
    //                                                     isRestricted);
    return true;
}

bool mtasa::ResourceMetaParser::ParseConfigFileNode(CXMLNode* node)
{
    CXMLAttributes& attributes = node->GetAttributes();

    CXMLAttribute* srcAttr = nullptr;
    CXMLAttribute* typeAttr = nullptr;

    for (auto iter = attributes.ListBegin(); iter != attributes.ListEnd(); ++iter)
    {
        CXMLAttribute*     attribute = *iter;
        const std::string& name = attribute->GetName();

        if (name == "src"s)
        {
            srcAttr = attribute;
        }
        else if (name == "type"s)
        {
            typeAttr = attribute;
        }
    }

    if (srcAttr == nullptr)
    {
        LogWarning("Missing 'src' attribute from 'config' node of 'meta.xml'"s);
        return false;
    }
    else if (srcAttr->GetValue().empty())
    {
        LogWarning("Empty 'src' attribute from 'config' node of 'meta.xml'"s);
        return false;
    }

    // TODO:
    // ReplaceSlashes(strFilename);
    // IsValidFilePath(strFilename.c_str())
    // GetFilePath(strFilename.c_str(), strFullFilename)

    // TODO: Duplicate path check
    // "Duplicate config file in resource '%s': '%s'\n"

    // TODO:
    // IF DUPLICATE: type = 0 is forced

    int type = 0;            // 0 = server, 1 = client, 2 = both

    if (typeAttr != nullptr)
    {
        const std::string& value = typeAttr->GetValue();

        if (!stricmp(value.c_str(), "shared"))
        {
            type = 2;
        }
        else if (!stricmp(value.c_str(), "client"))
        {
            type = 1;
        }
        else if (!stricmp(value.c_str(), "server"))
        {
            type = 0;
        }
        else
        {
            LogWarning(SString("Unknown 'type' attribute value '%.*s' (defaults to: server) from 'script' node of 'meta.xml'", value.size(), value.c_str()));
        }
    }

    // if (bServer)
    //     CResource::m_ResourceFiles.push_back(new CResourceConfigItem(this, strFilename.c_str(), strFullFilename.c_str(), &Attributes));
    // if (bClient)
    //     CResource::m_ResourceFiles.push_back(new CResourceClientConfigItem(this, strFilename.c_str(), strFullFilename.c_str(), &Attributes));
    return true;
}
