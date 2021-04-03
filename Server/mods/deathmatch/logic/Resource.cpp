/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Provide a resource from a directory
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "Resource.h"
#include "ResourceManager.h"
#include "MetaFileParser.h"

namespace fs = std::filesystem;

namespace mtasa
{
    static bool IsWindowsCompatiblePath(const fs::path& relativePath);

    Resource::Resource(ResourceManager& resourceManager) : m_resourceManager{resourceManager}
    {
        m_mapRootElement = g_pGame->GetMapManager()->GetRootElement();
    }

    bool Resource::Unload()
    {
        if (m_state != ResourceState::LOADED)
            return false;

        m_loadedTime = 0;
        m_state = ResourceState::NOT_LOADED;
        return true;
    }

    bool Resource::Load()
    {
        if (m_state != ResourceState::NOT_LOADED)
            return false;

        std::error_code errorCode;

        if (!fs::is_regular_file(m_metaFile, errorCode))
        {
            m_lastError = "Couldn't find meta.xml file for resource '"s + m_name + "'\n"s;
            CLogger::ErrorPrintf(m_lastError.c_str());
            return false;
        }

        MetaFileParser meta{m_name};
        std::string    parserError = meta.Parse(m_metaFile);

        if (!parserError.empty())
        {
            m_lastError =
                SString("Couldn't parse meta file for resource '%.*s' [%.*s]\n", m_name.size(), m_name.c_str(), parserError.size(), parserError.c_str());
            CLogger::ErrorPrintf(m_lastError.c_str());
            return false;
        }

        if (!ProcessMeta(meta))
            return false;

        time(&m_loadedTime);
        m_state = ResourceState::LOADED;
        return true;
    }

    bool Resource::Start()
    {
        if (m_state != ResourceState::LOADED)
            return false;

        m_state = ResourceState::STARTING;

        CLuaArguments preStartArguments;
        preStartArguments.PushResource(this);

        if (!m_mapRootElement->CallEvent("onResourcePreStart", preStartArguments))
        {
            m_lastError = "Start cancelled by script\n";
            m_state = ResourceState::LOADED;
            return false;
        }

        // TODO: Upgrade warnings
        
        // TODO: Compatibility check

        CGroups* entityGroup = g_pGame->GetGroups();

        m_element = new CDummy(entityGroup, m_mapRootElement);
        m_element->SetTypeName("resource");
        m_element->SetName(m_name);

        m_dynamicElementRoot = new CDummy(entityGroup, m_element);
        m_dynamicElementRoot->SetTypeName("map");
        m_dynamicElementRoot->SetName("dynamic");

        if (m_element->GetID() == INVALID_ELEMENT_ID || m_dynamicElementRoot->GetID() == INVALID_ELEMENT_ID)
        {
            delete m_element;
            m_element = nullptr;

            delete m_dynamicElementRoot;
            m_dynamicElementRoot = nullptr;

            m_state = ResourceState::LOADED;

            m_lastError = SString("Start up of resource %.*s cancelled by element id starvation\n", m_name.size(), m_name.c_str());
            CLogger::LogPrint(m_lastError.c_str());
            return false;
        }

        m_elementGroup = new CElementGroup();

        m_tempSettingsNode = g_pServerInterface->GetXML()->CreateDummyNode();

        // m_luaContext = g_pGame->GetLuaManager()->CreateLuaContext(reinterpret_cast<CResource*>(this), m_useOOP);
        // m_luaContext->SetScriptName(m_name.c_str());
        // TODO:
        // m_resourceManager.NotifyResourceVMOpen(this, m_luaContext);

        CLogger::LogPrintf(LOGLEVEL_LOW, "Starting %.*s\n", m_name.size(), m_name.c_str());
        time(&m_startedTime);

        // TODO:
        // if (m_bSyncMapElementDataDefined)
        //     m_pResourceManager->ApplySyncMapElementDataOption(this, m_bSyncMapElementData);

        // TODO: Start resource files

        m_state = ResourceState::RUNNING;

        CLuaArguments startArguments;
        startArguments.PushResource(this);

        if (!m_element->CallEvent("onResourceStart", startArguments))
        {
            m_lastError = "Start up of resource cancelled by script\n";
            CLogger::LogPrintf("Start up of resource %.*s cancelled by script\n", m_name.size(), m_name.c_str());

            Stop();

            return false;
        }

        // TODO: Generate checksums for files here

        // TODO:
        // g_pGame->GetMapManager()->BroadcastResourceElements(m_element, m_elementGroup);
        // g_pGame->GetPlayerManager()->BroadcastOnlyJoined(CResourceStartPacket(m_name.c_str(), this));
        // SendNoClientCacheScripts();

        return true;
    }

    bool Resource::Stop()
    {
        if (m_state != ResourceState::RUNNING)
            return false;

        m_state = ResourceState::STOPPING;
        // TODO: Add implementation here
        m_state = ResourceState::LOADED;
        return false;
    }

    fs::path Resource::GetUnsafeAbsoluteFilePath(const fs::path& relativePath)
    {
        if (relativePath.empty() || relativePath.is_absolute())
            return {};

        // First, always prefer our dynamic files directory
        std::error_code errorCode;
        fs::path        absolutePath = fs::canonical(m_dynamicDirectory / relativePath, errorCode);

        if (!errorCode && fs::is_regular_file(absolutePath, errorCode))
            return absolutePath;

        // Second, try again with our source files directory
        absolutePath = fs::canonical(m_sourceDirectory / relativePath, errorCode);

        if (!errorCode && fs::is_regular_file(absolutePath, errorCode))
            return absolutePath;

        // Third, default to our dynamic files directory
        return m_dynamicDirectory / relativePath;
    }

    bool Resource::SetInfoValue(const std::string& key, const std::string& value, bool persistChanges) { return false; }

    bool Resource::TryGetInfoValue(const std::string& key, std::string& value) const
    {
        if (auto iter = m_info.find(key); iter != m_info.end())
        {
            value = iter->second;
            return true;
        }

        return false;
    }

    bool Resource::RemoveInfoValue(const std::string& key, bool persistChanges) { return false; }

    bool Resource::ContainsSourceFile(const fs::path& relativePath) const
    {
        if (relativePath.is_absolute())
            return false;

        std::error_code errorCode;
        fs::path        absoluteFilePath = fs::canonical(m_sourceDirectory / relativePath, errorCode);

        if (errorCode)
            return false;

        if (!fs::is_regular_file(absoluteFilePath, errorCode))
            return false;

        // Check if the resulting absolute file path is inside our resource directory
        auto [rootIter, absoluteIter] = std::mismatch(m_sourceDirectory.begin(), m_sourceDirectory.end(), absoluteFilePath.begin(), absoluteFilePath.end());

        return (rootIter != m_sourceDirectory.end() && ++rootIter == m_sourceDirectory.end());
    }

    bool Resource::IsDuplicateServerFile(const fs::path& relativeFilePath)
    {
        return std::find(m_serverFiles.begin(), m_serverFiles.end(), relativeFilePath) != m_serverFiles.end();
    }

    bool Resource::IsDuplicateClientFile(const fs::path& relativeFilePath)
    {
        std::string lowercasePath = relativeFilePath.generic_string();
        std::transform(lowercasePath.begin(), lowercasePath.end(), lowercasePath.begin(), [](unsigned char c) { return tolower(c); });

        for (const auto& [path, string] : m_clientFiles)
        {
            if (lowercasePath == string || relativeFilePath == path)
                return true;
        }

        return false;
    }

    void Resource::AddServerFilePath(const fs::path& relativeFilePath)
    {
        m_serverFiles.push_back(relativeFilePath);
    }

    void Resource::AddClientFilePath(const fs::path& relativeFilePath)
    {
        std::string lowercasePath = relativeFilePath.generic_string();
        std::transform(lowercasePath.begin(), lowercasePath.end(), lowercasePath.begin(), [](unsigned char c) { return tolower(c); });
        m_clientFiles.push_back(std::make_pair(relativeFilePath, std::move(lowercasePath)));
    }

    bool Resource::ProcessMeta(const MetaFileParser& meta)
    {
        m_minServerVersion = meta.minServerVersion;
        m_metaMinServerVersion = meta.minServerVersion;

        m_minClientVersion = meta.minClientVersion;
        m_metaMinClientVersion = meta.minClientVersion;

        m_useOOP = meta.useOOP;
        m_downloadPriorityGroup = meta.downloadPriorityGroup;
        m_settingsNode = meta.settingsNode;

        if (meta.syncMapElementDataDefined)
        {
            // m_bSyncMapElementData = meta.syncMapElementData;
            // m_bSyncMapElementDataDefined = true;
        }
        else
        {
            // m_bSyncMapElementData = true;
            // m_bSyncMapElementDataDefined = false;
        }

        // TODO:
        /*
        // Find the acl requets
        CXMLNode* pNodeAclRequest = pRoot->FindSubNode("aclrequest", 0);

        if (pNodeAclRequest)
            RefreshAutoPermissions(pNodeAclRequest);
        else
            RemoveAutoPermissions();
        */

        for (const auto& [key, value] : meta.info)
        {
            m_info[key] = value;
        }

        return ProcessMetaIncludes(meta) && ProcessMetaMaps(meta) && ProcessMetaFiles(meta) && ProcessMetaScripts(meta) && ProcessMetaHtmls(meta) &&
               ProcessMetaExports(meta) && ProcessMetaConfigs(meta);
    }

    bool Resource::ProcessMetaIncludes(const MetaFileParser& meta)
    {
        for (const MetaDependencyItem& item : meta.dependencies)
        {
            // TODO:
            // SVersion minVersion{item.minVersion.major, item.minVersion.minor, item.minVersion.revision};
            // SVersion maxVersion{item.maxVersion.major, item.maxVersion.minor, item.maxVersion.revision};
            // m_IncludedResources.push_back(new CIncludedResources{m_pResourceManager, item.resourceName, minVersion, maxVersion, this});
        }

        return true;
    }

    bool Resource::ProcessMetaMaps(const MetaFileParser& meta)
    {
        for (const MetaFileItem& item : meta.maps)
        {
            std::string filePath = item.sourceFile.generic_string();

            if (!ContainsSourceFile(item.sourceFile))
            {
                m_lastError = SString("Couldn't find map %.*s for resource %.*s\n", filePath.size(), filePath.data(), m_name.size(), m_name.c_str());
                CLogger::ErrorPrintf(m_lastError.c_str());
                return false;
            }

            if (IsDuplicateServerFile(item.sourceFile))
            {
                CLogger::LogPrintf("WARNING: Duplicate map file in resource '%.*s': '%.*s'\n", m_name.size(), m_name.c_str(), filePath.size(), filePath.data());
            }

            // TODO:
            // m_ResourceFiles.push_back(new CResourceMapItem{this, filePath.c_str(), resourceFilePath->absolute.string().c_str(), nullptr, item.dimension});

            AddServerFilePath(item.sourceFile);
        }

        return true;
    }

    bool Resource::ProcessMetaFiles(const MetaFileParser& meta)
    {
        for (const MetaFileItem& item : meta.files)
        {
            std::string filePath = item.sourceFile.generic_string();

            if (!ContainsSourceFile(item.sourceFile))
            {
                m_lastError = SString("Couldn't find file %.*s for resource %.*s\n", filePath.size(), filePath.data(), m_name.size(), m_name.c_str());
                CLogger::ErrorPrintf(m_lastError.c_str());
                return false;
            }

            if (!IsWindowsCompatiblePath(item.sourceFile))
            {
                m_lastError = SString("Client file path %.*s for resource %.*s is not supported on Windows\n", filePath.size(), filePath.data(), m_name.size(),
                                      m_name.c_str());
                CLogger::ErrorPrintf(m_lastError.c_str());
                return false;
            }

            if (IsDuplicateClientFile(item.sourceFile))
            {
                CLogger::LogPrintf("WARNING: Ignoring duplicate client file in resource '%.*s': '%.*s'\n", m_name.size(), m_name.c_str(),
                                   filePath.size(), filePath.data());
                continue;
            }

            // TODO:
            // m_ResourceFiles.push_back(
            //     new CResourceClientFileItem{this, filePath.c_str(), resourceFilePath->absolute.string().c_str(), nullptr, !item.isClientOptional});

            AddClientFilePath(item.sourceFile);
        }

        return true;
    }

    bool Resource::ProcessMetaScripts(const MetaFileParser& meta)
    {
        for (const MetaFileItem& item : meta.scripts)
        {
            std::string filePath = item.sourceFile.generic_string();
            
            if (!ContainsSourceFile(item.sourceFile))
            {
                m_lastError = SString("Couldn't find script %.*s for resource %.*s\n", filePath.size(), filePath.data(), m_name.size(), m_name.c_str());
                CLogger::ErrorPrintf(m_lastError.c_str());
                return false;
            }

            if (item.isForClient && !IsWindowsCompatiblePath(item.sourceFile))
            {
                m_lastError = SString("Client script path %.*s for resource %.*s is not supported on Windows\n", filePath.size(), filePath.data(),
                                      m_name.size(), m_name.c_str());
                CLogger::ErrorPrintf(m_lastError.c_str());
                return false;
            }

            if (item.isForServer && IsDuplicateServerFile(item.sourceFile))
            {
                CLogger::LogPrintf("WARNING: Duplicate script file in resource '%.*s': '%.*s'\n", m_name.size(), m_name.c_str(), filePath.size(),
                                   filePath.data());
            }

            bool createForClient = item.isForClient;

            if (item.isForClient && IsDuplicateClientFile(item.sourceFile))
            {
                createForClient = false;

                CLogger::LogPrintf("WARNING: Ignoring duplicate client script file in resource '%.*s': '%.*s'\n", m_name.size(), m_name.c_str(),
                                   filePath.size(), filePath.data());
            }

            if (item.isForServer)
            {
                // TODO:
                // m_ResourceFiles.push_back(new CResourceScriptItem{this, filePath.c_str(), resourceFilePath->absolute.string().c_str(), nullptr});

                AddServerFilePath(item.sourceFile);
            }

            if (createForClient)
            {
                // TODO:
                // auto resourceFile = new CResourceClientScriptItem{this, filePath.c_str(), resourceFilePath->absolute.string().c_str(), nullptr};
                // resourceFile->SetNoClientCache(!item.isClientCacheable);
                // m_ResourceFiles.push_back(resourceFile);

                AddClientFilePath(item.sourceFile);
            }
        }

        return true;
    }

    bool Resource::ProcessMetaHtmls(const MetaFileParser& meta)
    {
        // TODO:
        // CResourceHTMLItem* firstHtmlFile = nullptr;
        // bool               hasDefaultHtmlPage = false;

        for (const MetaFileItem& item : meta.htmls)
        {
            std::string filePath = item.sourceFile.generic_string();
            
            if (!ContainsSourceFile(item.sourceFile))
            {
                std::string filePath = item.sourceFile.string();

                m_lastError = SString("Couldn't find html %.*s for resource %.*s\n", filePath.size(), filePath.data(), m_name.size(), m_name.c_str());
                CLogger::ErrorPrintf(m_lastError.c_str());
                return false;
            }

            if (IsDuplicateServerFile(item.sourceFile))
            {
                CLogger::LogPrintf("WARNING: Duplicate html file in resource '%.*s': '%.*s'\n", m_name.size(), m_name.c_str(), filePath.size(),
                                   filePath.data());
            }

            bool isDefault = item.isHttpDefault;

            if (isDefault)
            {
                // TODO:
                // if (hasDefaultHtmlPage)
                {
                    // isDefault = false;

                    // CLogger::LogPrintf("Only one html item can be default per resource, ignoring %.*s in %.*s\n", filePath.size(), filePath.data(),
                    //                    m_name.size(), m_name.c_str());
                }
                // else
                {
                    // hasDefaultHtmlPage = true;
                }
            }

            // TODO:
            // auto resourceFile = new CResourceHTMLItem{this,
            //                                           filePath.c_str(),
            //                                           resourceFilePath->absolute.string().c_str(),
            //                                           nullptr,
            //                                           isDefault,
            //                                           !!item.isHttpRaw,
            //                                           !!item.isHttpRestricted,
            //                                           m_bOOPEnabledInMetaXml};
            // 
            // m_ResourceFiles.push_back(resourceFile);

            // if (firstHtmlFile == nullptr)
            //     firstHtmlFile = resourceFile;

            AddServerFilePath(item.sourceFile);
        }

        // if (firstHtmlFile != nullptr && !hasDefaultHtmlPage)
        //     firstHtmlFile->SetDefaultPage(true);

        return true;
    }

    bool Resource::ProcessMetaExports(const MetaFileParser& meta)
    {
        for (const MetaExportItem& item : meta.exports)
        {
            if (item.isForServer)
            {
                // TODO:
                // m_ExportedFunctions.push_back(
                //     CExportedFunction{item.functionName, !!item.isHttpAccessible, CExportedFunction::EXPORTED_FUNCTION_TYPE_SERVER, !!item.isACLRestricted});
            }

            if (item.isForClient)
            {
                // TODO:
                // m_ExportedFunctions.push_back(
                //     CExportedFunction{item.functionName, !!item.isHttpAccessible, CExportedFunction::EXPORTED_FUNCTION_TYPE_CLIENT, !!item.isACLRestricted});
            }
        }

        return true;
    }

    bool Resource::ProcessMetaConfigs(const MetaFileParser& meta)
    {
        for (const MetaFileItem& item : meta.configs)
        {
            std::string filePath = item.sourceFile.generic_string();

            if (!ContainsSourceFile(item.sourceFile))
            {
                m_lastError = SString("Couldn't find config %.*s for resource %.*s\n", filePath.size(), filePath.data(), m_name.size(), m_name.c_str());
                CLogger::ErrorPrintf(m_lastError.c_str());
                return false;
            }

            if (item.isForClient && !IsWindowsCompatiblePath(item.sourceFile))
            {
                m_lastError = SString("Client config path %.*s for resource %.*s is not supported on Windows\n", filePath.size(), filePath.data(),
                                      m_name.size(), m_name.c_str());
                CLogger::ErrorPrintf(m_lastError.c_str());
                return false;
            }

            if (item.isForServer && IsDuplicateServerFile(item.sourceFile))
            {
                CLogger::LogPrintf("WARNING: Duplicate config file in resource '%.*s': '%.*s'\n", m_name.size(), m_name.c_str(), filePath.size(),
                                   filePath.data());
            }

            bool createForClient = item.isForClient;

            if (item.isForClient && IsDuplicateClientFile(item.sourceFile))
            {
                createForClient = false;

                CLogger::LogPrintf("WARNING: Ignoring duplicate client config file in resource '%.*s': '%.*s'\n", m_name.size(), m_name.c_str(),
                                   filePath.size(), filePath.data());
            }

            if (item.isForServer)
            {
                // TODO:
                // m_ResourceFiles.push_back(new CResourceConfigItem{this, filePath.c_str(), resourceFilePath->absolute.string().c_str(), nullptr});

                AddServerFilePath(item.sourceFile);
            }

            if (createForClient)
            {
                // TODO:
                // m_ResourceFiles.push_back(new CResourceClientConfigItem{this, filePath.c_str(), resourceFilePath->absolute.string().c_str(), nullptr});

                AddClientFilePath(item.sourceFile);
            }
        }

        return true;
    }

    static std::unordered_set<std::string> reservedWindowsFileNames = {"CON"s,  "PRN"s,  "AUX"s,  "NUL"s,  "COM1"s, "COM2"s, "COM3"s, "COM4"s,
                                                                       "COM5"s, "COM6"s, "COM7"s, "COM8"s, "COM9"s, "LPT1"s, "LPT2"s, "LPT3"s,
                                                                       "LPT4"s, "LPT5"s, "LPT6"s, "LPT7"s, "LPT8"s, "LPT9"s};

    // Check if the relative file path is allowed on a Microsoft Windows operating system
    // See https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file
    static bool IsWindowsCompatiblePath(const fs::path& relativePath)
    {
        std::string fileName = relativePath.filename().string();

        if (fileName.back() == '.' || fileName.back() == ' ')
        {
            return false;
        }
        else if (reservedWindowsFileNames.find(fileName) != reservedWindowsFileNames.end())
        {
            return false;
        }
        else
        {
            for (unsigned char c : fileName)
            {
                if (c < 32 || c == '<' || c == '>' || c == ':' || c == '"' || c == '/' || c == '\\' || c == '|' || c == '?' || c == '*')
                {
                    return false;
                }
            }
        }

        return true;
    }
}            // namespace mtasa
