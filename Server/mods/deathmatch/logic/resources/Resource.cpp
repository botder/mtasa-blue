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
#include "ResourceMapFile.h"
#include "ResourceHttpFile.h"
#include "ResourceScriptFile.h"
#include "ResourceConfigFile.h"
#include "ResourceClientFile.h"
#include "ResourceClientConfigFile.h"
#include "ResourceClientScriptFile.h"
#include "packets/CResourceStartPacket.h"
#include "packets/CResourceStopPacket.h"

namespace fs = std::filesystem;

namespace mtasa
{
    Resource::Resource(ResourceManager& resourceManager) : m_resourceManager{resourceManager} {}

    Resource::~Resource() = default;

    bool Resource::Unload()
    {
        if (m_state != ResourceState::LOADED)
            return false;

        std::exchange(m_serverFilePaths, {});
        std::exchange(m_clientFilePaths, {});
        std::exchange(m_resourceFiles, {});
        std::exchange(m_clientFunctions, {});
        std::exchange(m_serverFunctions, {});
        std::exchange(m_httpFiles, {});
        std::exchange(m_noCacheClientScripts, {});
        std::exchange(m_clientFiles, {});
        std::exchange(m_dependencies, {});
        std::exchange(m_info, {});
        m_settingsNode.reset();
        m_metaChecksum.Reset();
        m_minServerVersion = ""s;
        m_metaMinServerVersion = ""s;
        m_minClientVersion = ""s;
        m_metaMinClientVersion = ""s;
        m_downloadPriorityGroup = 0;
        m_usingOOP = false;
        m_loadedTime = 0;
        m_lastError = ""s;
        m_state = ResourceState::NOT_LOADED;
        return true;
    }

    bool Resource::Load()
    {
        if (m_state != ResourceState::NOT_LOADED)
            return false;

        std::error_code errorCode;
        fs::path        metaFile = m_sourceDirectory / "meta.xml";

        if (!fs::is_regular_file(metaFile, errorCode))
        {
            m_lastError = "Couldn't find meta.xml file for resource '"s + m_name + "'"s;
            CLogger::ErrorPrintf("%.*s\n", m_lastError.size(), m_lastError.c_str());
            return false;
        }

        if (!m_metaChecksum.Compute(metaFile))
        {
            m_lastError = "Couldn't compute meta.xml file checksum for resource '"s + m_name + "'"s;
            CLogger::ErrorPrintf("%.*s\n", m_lastError.size(), m_lastError.c_str());
            return false;
        }

        MetaFileParser meta{m_name};
        std::string    parserError = meta.Parse(metaFile);

        if (!parserError.empty())
        {
            m_lastError =
                SString("Couldn't parse meta file for resource '%.*s' [%.*s]", m_name.size(), m_name.c_str(), parserError.size(), parserError.c_str());
            CLogger::ErrorPrintf("%.*s\n", m_lastError.size(), m_lastError.c_str());
            return false;
        }

        if (!ProcessMeta(meta))
        {
            m_state = ResourceState::LOADED; // required for the call to `Resource::Unload`
            Unload();
            return false;
        }

        // Clean up the client cache directory, if it exists
        if (fs::exists(m_clientCacheDirectory, errorCode))
        {
            // Create a regular file list for the cache directory
            std::vector<fs::path> files;

            for (const fs::directory_entry& entry : fs::recursive_directory_iterator{m_clientCacheDirectory, errorCode})
            {
                if (entry.is_regular_file(errorCode))
                {
                    files.push_back(entry.path());
                }
            }

            // Try to delete the cache directory, if there are no regular files or if we have no cachable client files
            if (files.empty() || m_clientFiles.empty())
            {
                fs::remove(m_clientCacheDirectory, errorCode);
            }
            else
            {
                // Remove all cachable client files from the regular file list
                for (const ResourceFile* resourceFile : m_clientFiles)
                {
                    if (files.empty())
                        break;

                    fs::path cachePath{m_clientCacheDirectory / resourceFile->GetRelativePath()};
                    files.erase(std::remove(files.begin(), files.end(), cachePath), files.end());
                }

                // Delete the remaining files in the list
                for (const fs::path& filePath : files)
                {
                    fs::remove(filePath, errorCode);
                }
            }
        }

        time(&m_loadedTime);
        m_state = ResourceState::LOADED;
        return true;
    }

    bool Resource::Start(ResourceUseFlags useFlags)
    {
        if (m_state != ResourceState::LOADED)
            return false;

        m_state = ResourceState::STARTING;

        CDummy* mapRootElement = g_pGame->GetMapManager()->GetRootElement();

        CLuaArguments preStartArguments;
        preStartArguments.PushResource(this);

        if (!mapRootElement->CallEvent("onResourcePreStart", preStartArguments))
        {
            m_lastError = "Start cancelled by script";
            CLogger::LogPrintf("Start up of resource %.*s cancelled early by script\n", m_name.size(), m_name.c_str());
            m_state = ResourceState::LOADED;
            return false;
        }

        // CheckForIssues
        // if (!m_bDoneUpgradeWarnings)
        // {
        //     m_bDoneUpgradeWarnings = true;
        //     CResourceChecker().LogUpgradeWarnings(this, m_archiveFilePath.string(), m_strMinClientReqFromSource, m_strMinServerReqFromSource,
        //                                           m_strMinClientReason, m_strMinServerReason);
        // }

        // MTA version check
        // SString strStatus;
        // 
        // if (!GetCompatibilityStatus(strStatus))
        // {
        //     m_strFailureReason = SString("Not starting resource %s as %s\n", m_strResourceName.c_str(), strStatus.c_str());
        //     CLogger::LogPrint(m_strFailureReason);
        //     m_eState = EResourceState::Loaded;
        //     return false;
        // }
        // else if (!strStatus.empty())
        // {
        //     SString strReason = SString("WARNING: %s requires upgrade as %s\n", m_strResourceName.c_str(), *strStatus);
        //     CLogger::LogPrint(strReason);
        //     CLogger::LogPrintf("Use the 'upgrade' command to perform a basic upgrade of resources.\n");
        // }

        // Check the included resources are linked
        // if (!m_bLinked)
        // {
        //     if (!LinkToIncludedResources())
        //     {
        //         m_eState = EResourceState::Loaded;
        //         return false;
        //     }
        // }

        CGroups* entityGroup = g_pGame->GetGroups();

        auto resourceRoot = std::make_unique<CDummy>(entityGroup, mapRootElement);
        resourceRoot->SetTypeName("resource");
        resourceRoot->SetName(m_name);

        auto dynamicElementRoot = std::make_unique<CDummy>(entityGroup, resourceRoot.get());
        resourceRoot->SetTypeName("map");
        resourceRoot->SetName("dynamic");

        if (resourceRoot->GetID() == INVALID_ELEMENT_ID || dynamicElementRoot->GetID() == INVALID_ELEMENT_ID)
        {
            m_state = ResourceState::LOADED;

            m_lastError = SString("Start up of resource %.*s cancelled by element id starvation", m_name.size(), m_name.c_str());
            CLogger::LogPrintf("%.*s\n", m_lastError.size(), m_lastError.c_str());
            return false;
        }

        if (!ComputeFileMetaDatum() || IsAnyResourceFileBlocked())
        {
            m_state = ResourceState::LOADED;
            CLogger::LogPrintf("Start up of resource %.*s cancelled by server\n", m_name.size(), m_name.c_str());
            return false;
        }

        m_element = resourceRoot.release();
        m_dynamicElementRoot = dynamicElementRoot.release();

        m_elementGroup = new CElementGroup();

        // m_tempSettingsNode = g_pServerInterface->GetXML()->CreateDummyNode();

        CreateLuaContext();

        // TODO:
        // if (m_bSyncMapElementDataDefined)
        //     m_pResourceManager->ApplySyncMapElementDataOption(this, m_bSyncMapElementData);

        time(&m_startedTime);

        CLogger::LogPrintf(LOGLEVEL_LOW, "Starting %.*s\n", m_name.size(), m_name.c_str());

        bool hasResourceFileError = false;

        for (std::unique_ptr<ResourceFile>& file : m_resourceFiles)
        {
            bool ignoreFile = false;

            switch (file->GetType())
            {
                case ResourceFileType::CLIENT_FILE:
                    if (!useFlags.useClientFiles)
                        ignoreFile = true;
                    break;
                case ResourceFileType::CLIENT_SCRIPT:
                    if (!useFlags.useClientScripts)
                        ignoreFile = true;
                    break;
                case ResourceFileType::CLIENT_CONFIG:
                    if (!useFlags.useClientConfigs)
                        ignoreFile = true;
                    break;
                case ResourceFileType::SERVER_MAP:
                    if (!useFlags.useServerMaps)
                        ignoreFile = true;
                    break;
                case ResourceFileType::SERVER_HTTP:
                    if (!useFlags.useHttpFiles)
                        ignoreFile = true;
                    break;
                case ResourceFileType::SERVER_CONFIG:
                    if (!useFlags.useServerConfigs)
                        ignoreFile = true;
                    break;
                case ResourceFileType::SERVER_SCRIPT:
                    if (!useFlags.useServerScripts)
                        ignoreFile = true;
                    break;
                default:
                    ignoreFile = true;
            }

            if (ignoreFile)
                continue;

            if (!file->Start())
            {
                hasResourceFileError = true;

                const std::string& name = file->GetName();
                m_lastError = SString("Failed to start resource item %.*s which is required", name.size(), name.c_str());
                CLogger::LogPrintf("Failed to start resource item %.*s in %.*s\n", name.size(), name.c_str(), m_name.size(), m_name.c_str());

                break;
            }
        }

        if (hasResourceFileError)
        {
            for (std::unique_ptr<ResourceFile>& file : m_resourceFiles)
            {
                file->Stop();
            }

            DeleteLuaContext();
            m_startedTime = 0;

            // delete m_tempSettingsNode;
            // m_tempSettingsNode = nullptr;

            delete m_elementGroup;
            m_elementGroup = nullptr;

            g_pGame->GetElementDeleter()->Delete(m_element);
            m_element = nullptr;

            g_pGame->GetElementDeleter()->Delete(m_dynamicElementRoot);
            m_dynamicElementRoot = nullptr;
            
            m_state = ResourceState::LOADED;
            return false;
        }

        if (useFlags.useDependencies)
        {
            for (ResourceDependency& dependency : m_dependencies)
            {
                // TODO: Add implementation here
                // Shouldn't we start dependencies before starting our stuff?
            }
        }

        // if (StartOptions.bIncludedResources)
        // {
        //     // Copy the list over included resources because reloading them might change the list
        //     std::list<CIncludedResources*> includedResources = m_IncludedResources;
        // 
        //     // Start our included resources that haven't been started
        //     for (CIncludedResources* pIncludedResources : includedResources)
        //     {
        //         // Has it already been loaded?
        //         CResource* pIncluded = pIncludedResources->GetResource();
        // 
        //         if (pIncluded)
        //         {
        //             // Included resource has changed?
        //             if (pIncluded->HasResourceChanged())
        //             {
        //                 // Reload it if it's not already started
        //                 if (!pIncluded->IsActive())
        //                 {
        //                     m_pResourceManager->Reload(pIncluded);
        //                 }
        //                 else
        //                 {
        //                     CLogger::LogPrintf("WARNING: Included resource %s has changed but unable to reload due to resource already being in use\n",
        //                                        pIncluded->GetName().c_str());
        //                 }
        //             }
        // 
        //             // Make us dependant of it
        //             pIncluded->AddDependent(this);
        //         }
        //     }
        // }

        // Add the resources depending on us
        // if (pDependents)
        // {
        //     for (CResource* pDependent : *pDependents)
        //         AddDependent(pDependent);
        // }

        m_state = ResourceState::RUNNING;
        m_useFlags = useFlags;

        CLuaArguments startArguments;
        startArguments.PushResource(this);

        if (!m_element->CallEvent("onResourceStart", startArguments))
        {
            m_lastError = "Start up of resource cancelled by script";
            CLogger::LogPrintf("Start up of resource %.*s cancelled by script\n", m_name.size(), m_name.c_str());
            Stop();
            return false;
        }

        // TODO:
        // m_pResourceManager->ApplyMinClientRequirement(this, m_strMinClientRequirement);
        g_pGame->GetMapManager()->BroadcastResourceElements(m_element, m_elementGroup);
        g_pGame->GetPlayerManager()->BroadcastOnlyJoined(CResourceStartPacket{*this});
        // SendNoClientCacheScripts();
        m_syncElementsToClients = true;
        // m_pResourceManager->OnResourceStart(this);

        return true;
    }

    bool Resource::Stop()
    {
        if (m_state != ResourceState::RUNNING)
            return false;

        // Tell the log that we've stopped this resource
        CLogger::LogPrintf(LOGLEVEL_LOW, "Stopping %.*s\n", m_name.size(), m_name.c_str());

        m_state = ResourceState::STOPPING;
        m_startedTime = 0;
        m_syncElementsToClients = false;
        m_useFlags = {};

        // m_pResourceManager->OnResourceStop(this);
        // m_pResourceManager->RemoveMinClientRequirement(this);
        // m_pResourceManager->RemoveSyncMapElementDataOption(this);

        // Tell the modules we are stopping
        g_pGame->GetLuaManager()->GetLuaModuleManager()->ResourceStopping(m_luaContext->GetMainLuaState());

        // Tell all the players that have joined that this resource is stopped
        g_pGame->GetPlayerManager()->BroadcastOnlyJoined(CResourceStopPacket(m_remoteIdentifier));

        // Call the onResourceStop event on this resource element
        CLuaArguments Arguments;
        Arguments.PushResource(this);
        Arguments.PushBoolean(m_wasDeleted);
        m_element->CallEvent("onResourceStop", Arguments);

        /* Remove us from the resources we depend on (they might unload too first)
        for (CIncludedResources* pIncludedResources : m_IncludedResources)
        {
            CResource* pResource = pIncludedResources->GetResource();

            if (pResource)
                pResource->RemoveDependent(this);
        }

        // Temorary includes??
        for (CResource* pResource : m_TemporaryIncludes)
            pResource->RemoveDependent(this);

        m_TemporaryIncludes.clear();*/

        for (std::unique_ptr<ResourceFile>& file : m_resourceFiles)
        {
            file->Stop();
        }

        // Tell the module manager we have stopped
        g_pGame->GetLuaManager()->GetLuaModuleManager()->ResourceStopped(m_luaContext->GetMainLuaState());

        // Remove the temporary XML storage node
        // if (m_tempSettingsNode != nullptr)
        // {
        //     delete m_tempSettingsNode;
        //     m_tempSettingsNode = nullptr;
        // }

        // Destroy the element group attached directly to this resource
        if (m_elementGroup != nullptr)
        {
            delete m_elementGroup;
            m_elementGroup = nullptr;
        }

        // Destroy the virtual machine for this resource
        DeleteLuaContext();

        // Remove the resource element from the client
        CEntityRemovePacket removePacket;

        if (m_element != nullptr)
        {
            removePacket.Add(m_element);
            g_pGame->GetElementDeleter()->Delete(m_element);
            m_element = nullptr;
        }

        // Remove the dynamic resource element from the client
        if (m_dynamicElementRoot != nullptr)
        {
            removePacket.Add(m_dynamicElementRoot);
            g_pGame->GetElementDeleter()->Delete(m_dynamicElementRoot);
            m_dynamicElementRoot = nullptr;
        }

        // Broadcast the packet to joined players
        g_pGame->GetPlayerManager()->BroadcastOnlyJoined(removePacket);

        m_wasDeleted = false;
        m_state = ResourceState::LOADED;
        return true;
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

    bool Resource::CallExportedFunction(const std::string& functionName, CLuaArguments& arguments, CLuaArguments& returnValues, Resource& sourceResource)
    {
        if (m_state != ResourceState::RUNNING)
            return false;

        // Check if the ACL right name fits into 512 bytes (-1 for null byte)
        // ACL right name pattern: <resourceName>.function.<functionName>
        std::size_t requiredBufferSize = m_name.size() + functionName.size() + 10;

        if (requiredBufferSize > 511)
            return false;

        if (auto iter = m_serverFunctions.find(functionName); iter != m_serverFunctions.end())
        {
            std::string functionRight;
            functionRight.reserve(requiredBufferSize);
            functionRight += m_name;
            functionRight += ".function."sv;
            functionRight += functionName;

            CAccessControlListManager* aclManager = g_pGame->GetACLManager();
            const ResourceServerFunction&      function = iter->second;
            const std::string&         sourceResourceName = sourceResource.GetName();

            if (aclManager->CanObjectUseRight(sourceResourceName.c_str(), CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE, m_name.c_str(),
                                              CAccessControlListRight::RIGHT_TYPE_RESOURCE, !function.isACLRestricted) &&
                aclManager->CanObjectUseRight(sourceResourceName.c_str(), CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE, functionRight.c_str(),
                                              CAccessControlListRight::RIGHT_TYPE_RESOURCE, !function.isACLRestricted))
            {
                if (arguments.CallGlobal(m_luaContext, functionName.c_str(), &returnValues))
                {
                    return true;
                }
            }
        }

        return false;
    }

    std::vector<std::string_view> Resource::GetExportedServerFunctions() const
    {
        std::vector<std::string_view> result;

        for (const auto& [functionName, function] : m_serverFunctions)
            result.push_back(functionName);

        return result;
    }

    std::vector<std::string_view> Resource::GetExportedClientFunctions() const
    {
        std::vector<std::string_view> result;

        for (const ResourceClientFunction& function : m_clientFunctions)
            result.push_back(function.name);

        return result;
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

    bool Resource::Exists() const
    {
        std::error_code errorCode;
        return fs::is_regular_file(m_sourceDirectory / "meta.xml", errorCode);
    }

    bool Resource::HasChanged() const
    {
        if (m_state == ResourceState::RUNNING)
        {
            for (const std::unique_ptr<ResourceFile>& resourceFile : m_resourceFiles)
            {
                if (resourceFile->HasChanged())
                    return true;
            }
        }

        return m_metaChecksum.HasChanged(m_sourceDirectory / "meta.xml");
    }

    bool Resource::SourceFileExists(const fs::path& relativePath) const
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
        auto [iter, ignore] = std::mismatch(m_sourceDirectory.begin(), m_sourceDirectory.end(), absoluteFilePath.begin(), absoluteFilePath.end());
        return iter == m_sourceDirectory.end();
    }

    bool Resource::ComputeFileMetaDatum()
    {
        if (m_resourceFiles.empty())
            return true;

        unsigned int batchSize = std::max(std::thread::hardware_concurrency(), 8u);

        using FutureResult = std::pair<ResourceFile*, bool>;
        std::vector<std::future<FutureResult>> tasks;
        tasks.reserve(batchSize);

        for (std::size_t i = 0; i < m_resourceFiles.size(); /* manual increment */)
        {
            for (unsigned int b = 0; b < batchSize && i < m_resourceFiles.size(); i++, b++)
            {
                ResourceFile*             file = m_resourceFiles[i].get();
                std::future<FutureResult> task = SharedUtil::async([file] { return std::make_pair(file, file->ComputeSourceFileMetaData()); });
                tasks.push_back(std::move(task));
            }

            bool failure = false;

            for (std::future<FutureResult>& task : tasks)
            {
                auto [resourceFile, successful] = task.get();

                if (successful)
                    continue;

                failure = true;

                m_lastError = "File '" + resourceFile->GetName();

                if (resourceFile->SourceFileExists())
                    m_lastError += "' is unreadable";
                else
                    m_lastError += "' does not exist";

                CLogger::LogPrintf("%.*s\n", m_lastError.size(), m_lastError.c_str());
            }

            if (failure)
                return false;

            tasks.clear();
        }

        return true;
    }

    bool Resource::IsAnyResourceFileBlocked()
    {
        bool result = false;

        for (const std::unique_ptr<ResourceFile>& file : m_resourceFiles)
        {
            std::string reason = m_resourceManager.GetBlockedFileReason(file->GetSourceChecksum());

            if (!reason.empty())
            {
                result = true;

                const std::string& name = file->GetName();
                m_lastError = SString("File '%.*s' is blocked (%.*s)", name.size(), name.c_str(), reason.size(), reason.c_str());
                CLogger::LogPrintf("Failed to start resource '%.*s' - %.*s\n", m_name.size(), m_name.c_str(), m_lastError.size(), m_lastError.c_str());
            }
        }

        return result;
    }

    bool Resource::CreateLuaContext()
    {
        CLuaManager* luaManager = g_pGame->GetLuaManager();

        if (m_luaContext = luaManager->CreateLuaContext(*this); m_luaContext == nullptr)
            return false;

        LuaContextUseFlags useFlags;
        useFlags.useOOP = m_usingOOP;

        if (m_luaContext->OpenMainLuaState(useFlags))
            return true;

        luaManager->DeleteLuaContext(m_luaContext);
        m_luaContext = nullptr;
        return false;
    }

    void Resource::DeleteLuaContext()
    {
        g_pGame->GetLuaManager()->DeleteLuaContext(m_luaContext);
        m_luaContext = nullptr;
    }

    bool Resource::IsDuplicateServerFile(const fs::path& relativeFilePath)
    {
        return std::find(m_serverFilePaths.begin(), m_serverFilePaths.end(), relativeFilePath) != m_serverFilePaths.end();
    }

    bool Resource::IsDuplicateClientFile(const fs::path& relativeFilePath)
    {
        std::string lowercasePath = relativeFilePath.generic_string();
        std::transform(lowercasePath.begin(), lowercasePath.end(), lowercasePath.begin(), [](unsigned char c) { return tolower(c); });

        for (const auto& [path, string] : m_clientFilePaths)
        {
            if (lowercasePath == string || relativeFilePath == path)
                return true;
        }

        return false;
    }

    void Resource::AddServerFilePath(const fs::path& relativeFilePath)
    {
        m_serverFilePaths.push_back(relativeFilePath);
    }

    void Resource::AddClientFilePath(const fs::path& relativeFilePath)
    {
        std::string lowercasePath = relativeFilePath.generic_string();
        std::transform(lowercasePath.begin(), lowercasePath.end(), lowercasePath.begin(), [](unsigned char c) { return tolower(c); });
        m_clientFilePaths.push_back(std::make_pair(relativeFilePath, std::move(lowercasePath)));
    }

    bool Resource::ProcessMeta(const MetaFileParser& meta)
    {
        m_minServerVersion = meta.minServerVersion;
        m_metaMinServerVersion = meta.minServerVersion;

        m_minClientVersion = meta.minClientVersion;
        m_metaMinClientVersion = meta.minClientVersion;

        m_usingOOP = meta.useOOP;
        m_downloadPriorityGroup = meta.downloadPriorityGroup;
        m_settingsNode = std::unique_ptr<CXMLNode>(meta.settingsNode);

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

        ProcessMetaIncludes(meta);
        ProcessMetaExports(meta);

        if (!ProcessMetaFiles(meta) && !m_lastError.empty())
        {
            CLogger::ErrorPrintf("%.*s\n", m_lastError.size(), m_lastError.c_str());
            return false;
        }

        return true;
    }

    void Resource::ProcessMetaIncludes(const MetaFileParser& meta)
    {
        for (const MetaDependencyItem& item : meta.dependencies)
        {
            ResourceDependency dependency;
            dependency.resourceName = item.resourceName;
            dependency.minVersion.major = item.minVersion.major;
            dependency.minVersion.minor = item.minVersion.minor;
            dependency.minVersion.revision = item.minVersion.revision;
            dependency.maxVersion.major = item.maxVersion.major;
            dependency.maxVersion.minor = item.maxVersion.minor;
            dependency.maxVersion.revision = item.maxVersion.revision;

            m_dependencies.push_back(std::move(dependency));
        }
    }
    
    void Resource::ProcessMetaExports(const MetaFileParser& meta)
    {
        for (const MetaExportItem& item : meta.exports)
        {
            if (item.isForServer)
            {
                ResourceServerFunction function;
                function.isHttpAccessible = item.isHttpAccessible;
                function.isACLRestricted = item.isACLRestricted;

                m_serverFunctions[item.functionName] = std::move(function);
            }

            if (item.isForClient)
            {
                ResourceClientFunction function;
                function.name = item.functionName;
                function.isHttpAccessible = item.isHttpAccessible;
                function.isACLRestricted = item.isACLRestricted;

                m_clientFunctions.push_back(std::move(function));
            }
        }
    }

    bool Resource::ProcessMetaFiles(const MetaFileParser& meta)
    {
        bool hasDefaultHtmlPage = false;

        for (const MetaFileItem& item : meta.items)
        {
            std::string      filePath = item.sourceFile.generic_string();
            std::string_view typeString = MetaFileItemTypeToString(item.type);

            if (!SourceFileExists(item.sourceFile))
            {
                // Couldn't find <typeString> <filePath> for resource <resourceName>
                m_lastError = SString("Couldn't find %.*s %.*s for resource %.*s", typeString.size(), typeString.data(), filePath.size(), filePath.data(),
                                      m_name.size(), m_name.c_str());
                return false;
            }

            if (item.isForClient && !IsWindowsCompatiblePath(item.sourceFile))
            {
                // Client <typeString> path <filePath> for resource <resourceName> is not supported on Windows
                m_lastError = SString("Client %.*s path %.*s for resource %.*s is not supported on Windows", typeString.size(), typeString.data(),
                                      filePath.size(), filePath.data(), m_name.size(), m_name.c_str());
                return false;
            }

            bool createForServer = item.isForServer;

            if (item.isForServer)
            {
                if (IsDuplicateServerFile(item.sourceFile))
                {
                    // WARNING: Duplicate <typeString> file in resource '<resourceName>': '<filePath>'
                    CLogger::LogPrintf("WARNING: Duplicate %.*s file in resource '%.*s': '%.*s'\n", typeString.size(), typeString.data(), m_name.size(),
                                       m_name.c_str(), filePath.size(), filePath.data());
                }
                else
                {
                    AddServerFilePath(item.sourceFile);
                }
            }

            bool createForClient = item.isForClient;

            if (item.isForClient)
            {
                if (IsDuplicateClientFile(item.sourceFile))
                {
                    createForClient = false;

                    // WARNING: Ignoring duplicate client <typeName> file in resource '<resourceName>': '<filePath>'
                    CLogger::LogPrintf("WARNING: Ignoring duplicate client %.*s file in resource '%.*s': '%.*s'\n", typeString.size(), typeString.data(),
                                       m_name.size(), m_name.c_str(), filePath.size(), filePath.data());
                }
                else
                {
                    AddClientFilePath(item.sourceFile);
                }
            }

            switch (item.type)
            {
                case MetaFileItemType::MAP:
                {
                    if (!createForServer)
                        break;

                    auto file = std::make_unique<ResourceMapFile>(*this);
                    file->SetRelativePath(item.sourceFile);
                    file->SetDimension(item.dimension);

                    m_resourceFiles.push_back(std::move(file));
                    break;
                }
                case MetaFileItemType::FILE:
                {
                    if (!createForClient)
                        break;

                    auto file = std::make_unique<ResourceClientFile>(*this);
                    file->SetRelativePath(item.sourceFile);
                    file->SetIsOptional(item.isClientOptional);

                    m_clientFiles.push_back(file.get());
                    m_resourceFiles.push_back(std::move(file));
                    break;
                }
                case MetaFileItemType::SCRIPT:
                {
                    if (createForServer)
                    {
                        auto file = std::make_unique<ResourceScriptFile>(*this);
                        file->SetRelativePath(item.sourceFile);

                        m_resourceFiles.push_back(std::move(file));
                    }

                    if (createForClient)
                    {
                        auto file = std::make_unique<ResourceClientScriptFile>(*this, item.isClientCacheable);
                        file->SetRelativePath(item.sourceFile);

                        if (item.isClientCacheable)
                            m_clientFiles.push_back(file.get());
                        else
                            m_noCacheClientScripts.push_back(file.get());

                        m_resourceFiles.push_back(std::move(file));
                    }
                    break;
                }
                case MetaFileItemType::HTML:
                {
                    if (!createForServer)
                        break;

                    bool isDefault = item.isHttpDefault;

                    if (isDefault)
                    {
                        if (hasDefaultHtmlPage)
                            isDefault = false;
                        else
                            hasDefaultHtmlPage = true;

                        if (!isDefault)
                        {
                            // Only one html item can be default per resource, ignoring <filePath> in <resourceName>
                            CLogger::LogPrintf("Only one html item can be default per resource, ignoring %.*s in %.*s\n", filePath.size(), filePath.data(),
                                               m_name.size(), m_name.c_str());
                        }
                    }

                    auto file = std::make_unique<ResourceHttpFile>(*this);
                    file->SetRelativePath(item.sourceFile);
                    file->SetIsRaw(item.isHttpRaw);
                    file->SetIsDefault(isDefault);
                    file->SetIsACLRestricted(item.isHttpRestricted);
                    file->SetIsUsingOOP(m_usingOOP);

                    m_httpFiles.push_back(file.get());
                    m_resourceFiles.push_back(std::move(file));
                    break;
                }
                case MetaFileItemType::CONFIG:
                {
                    if (createForServer)
                    {
                        auto file = std::make_unique<ResourceConfigFile>(*this);
                        file->SetRelativePath(item.sourceFile);

                        m_resourceFiles.push_back(std::move(file));
                    }

                    if (createForClient)
                    {
                        auto file = std::make_unique<ResourceClientConfigFile>(*this);
                        file->SetRelativePath(item.sourceFile);

                        m_clientFiles.push_back(file.get());
                        m_resourceFiles.push_back(std::move(file));
                    }
                    break;
                }
            }
        }

        if (!hasDefaultHtmlPage && !m_httpFiles.empty())
            m_httpFiles.front()->SetIsDefault(true);

        return true;
    }
}            // namespace mtasa
