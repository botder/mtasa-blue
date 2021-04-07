/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource lifetime manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "ArchiveResource.h"
#include "ResourceCommands.h"

namespace fs = std::filesystem;

namespace mtasa
{
    template <typename I>
    struct NumberGenerator
    {
        I storage = 0;

        constexpr I operator()() { return ++storage; }
    };

    enum class ResourceLocationType
    {
        DIRECTORY,
        ZIP,
    };

    struct ResourceLocation
    {
        ResourceLocationType type = ResourceLocationType::DIRECTORY;
        fs::path             absolutePath;
        fs::path             relativePath;
        std::string          resourceName;
    };

    using ResourceLocations = std::vector<ResourceLocation>;
    using ResourceNameToLocations = std::unordered_map<std::string, ResourceLocations, LowercaseHash, LowercaseEqual>;

    void CreateDirectories(const fs::path& directory);
    void CreateResourceCacheReadme(const fs::path& directory);

    ResourceNameToLocations LocateResources(const fs::path& resourcesDirectory);
    ResourceLocations       FilterForUniqueResources(const ResourceNameToLocations& resourceCandidates);

    ResourceManager::ResourceManager(const fs::path& baseDirectory)
    {
        m_unusedResourceRemoteIdentifiers.resize(INVALID_RESOURCE_REMOTE_ID - 1);
        std::generate(m_unusedResourceRemoteIdentifiers.rbegin(), m_unusedResourceRemoteIdentifiers.rend(), NumberGenerator<std::uint16_t>{});

        m_resourcesDirectory = baseDirectory / RESOURCES_DIRECTORY_NAME;

        fs::path resourceCacheDirectory = baseDirectory / CACHE_DIRECTORY_NAME;
        m_archiveDecompressionDirectory = resourceCacheDirectory / ARCHIVE_DECOMPRESSION_DIRECTORY_NAME;

        CreateDirectories(m_archiveDecompressionDirectory);
        CreateResourceCacheReadme(resourceCacheDirectory);
    }

    ResourceManager::~ResourceManager() = default;

    void ResourceManager::Shutdown()
    {
        m_commandsQueue.clear();

        for (Resource* resource : std::exchange(m_resources, {}))
        {
            DestroyResource(resource);
        }
    }

    Resource* ResourceManager::GetResourceFromName(std::string_view resourceName)
    {
        if (auto iter = m_nameToResource.find(std::string{resourceName}); iter != m_nameToResource.end())
            return iter->second;

        return nullptr;
    }

    Resource* ResourceManager::GetResourceFromScriptIdentifier(SArrayId id)
    {
        if (auto iter = m_scriptIdToResource.find(id); iter != m_scriptIdToResource.end())
            return iter->second;

        return nullptr;
    }

    Resource* ResourceManager::GetResourceFromRemoteIdentifier(std::uint16_t id)
    {
        if (auto iter = m_remoteIdToResource.find(id); iter != m_remoteIdToResource.end())
            return iter->second;

        return nullptr;
    }

    CreateResourceError ResourceManager::TryCreateResource(std::string_view resourceName, std::string_view newGroupDirectory, Resource*& newResource)
    {
        // TODO: Add implementation here
        return CreateResourceError::DUMMY_FAIL;
    }

    RenameResourceError ResourceManager::TryRenameResource(Resource* resource, std::string_view newResourceName, std::string_view newGroupDirectory)
    {
        // TODO: Add implementation here
        return RenameResourceError::DUMMY_FAIL;
    }

    CloneResourceError ResourceManager::TryCloneResource(Resource* resource, std::string_view newResourceName, std::string_view newGroupDirectory,
                                                         Resource*& newResource)
    {
        // TODO: Add implementation here
        return CloneResourceError::DUMMY_FAIL;
    }

    void ResourceManager::QueueResourceCommand(std::unique_ptr<ResourceCommand> command)
    {
        m_commandsQueue.push_back(std::move(command));
    }

    void ResourceManager::RefreshResources(bool includeRunningResources)
    {
        std::vector<Resource*> resources{m_resources};

        for (Resource* resource : resources)
        {
            if (g_pServerInterface->IsRequestingExit())
                return;

            if (!includeRunningResources && resource->IsRunning())
                continue;

            RefreshResource(resource);
        }

        SearchForNewResources();
    }

    void ResourceManager::RefreshResource(Resource* resource)
    {
        if (resource->GetState() == ResourceState::NOT_LOADED)
        {
            // Try to load resource again, because it had loading issues last time
            LoadErroneousResource(resource);
        }
        else if (!resource->Exists())
        {
            // Destroy resource, because the source files don't exist anymore
            DestroyZombieResource(resource);
        }
        else if (resource->HasChanged())
        {
            // Reload resource, because the source files have changed
            ReloadChangedResource(resource);
        }
    }

    void ResourceManager::StopResources()
    {
        for (Resource* resource : m_resources)
        {
            resource->Stop();
        }
    }

    void ResourceManager::ProcessQueue()
    {
        if (m_isRefreshQueued)
        {
            bool includeRunningResources = m_includeRunningResourcesRefresh;

            m_isRefreshQueued = false;
            m_includeRunningResourcesRefresh = false;

            RefreshResources(includeRunningResources);
        }

        while (!m_commandsQueue.empty())
        {
            std::unique_ptr<ResourceCommand> command = std::move(m_commandsQueue.front());
            m_commandsQueue.pop_front();

            if (command->resource == nullptr)
                continue;

            command->Execute();
        }
    }

    bool ResourceManager::MoveResourceToTrash(Resource* resource)
    {
        // TODO: Add implementation here
        return false;
    }

    void ResourceManager::OnPlayerJoin(CPlayer& player)
    {
        // TODO: Add implementation here
    }

    std::string ResourceManager::GetBlockedFileReason(const ResourceFileChecksum& checksum)
    {
        // TODO: Add implementation here
        // std::array<char, 32> hash = CMD5Hasher::ConvertToHexArray(checksum.GetMD5());
        return {};
    }

    SArrayId ResourceManager::GenerateResourceScriptIdentifier()
    {
        return CIdArray::PopUniqueId(this, EIdClass::RESOURCE);
    }

    void ResourceManager::RecycleResourceScriptIdentifier(SArrayId id)
    {
        CIdArray::PushUniqueId(this, EIdClass::RESOURCE, id);
    }

    std::uint16_t ResourceManager::GenerateResourceRemoteIdentifier()
    {
        if (m_unusedResourceRemoteIdentifiers.empty())
            return INVALID_RESOURCE_REMOTE_ID;

        std::uint16_t remoteIdentifier = m_unusedResourceRemoteIdentifiers.back();
        m_unusedResourceRemoteIdentifiers.pop_back();
        return remoteIdentifier;
    }

    void ResourceManager::RecycleResourceRemoteIdentifier(std::uint16_t id)
    {
        m_unusedResourceRemoteIdentifiers.push_back(id);
    }

    void ResourceManager::DestroyResource(Resource* resource)
    {
        if (resource->GetState() != ResourceState::NOT_LOADED)
        {
            resource->Stop();
            resource->Unload();
            --m_numLoadedResources;
        }
        else
        {
            --m_numErroneousResources;
        }

        SArrayId scriptId = resource->GetScriptIdentifier();
        RecycleResourceScriptIdentifier(scriptId);

        std::uint16_t remoteId = resource->GetRemoteIdentifier();
        RecycleResourceRemoteIdentifier(remoteId);

        m_remoteIdToResource.erase(remoteId);
        m_scriptIdToResource.erase(scriptId);
        m_nameToResource.erase(resource->GetName());

        if (!m_resources.empty())
        {
            m_resources.erase(std::remove(m_resources.begin(), m_resources.end(), resource));
        }

        if (!m_commandsQueue.empty())
        {
            for (auto iter = m_commandsQueue.begin(); iter != m_commandsQueue.end(); /* manual increment */)
            {
                if ((*iter)->resource == resource)
                    iter = m_commandsQueue.erase(iter);
                else
                    ++iter;
            }
        }

        delete resource;
    }

    void ResourceManager::SearchForNewResources()
    {
        ResourceNameToLocations resourceCandidates = LocateResources(m_resourcesDirectory);

        if (resourceCandidates.empty())
            return;

        ResourceLocations resourceLocations = FilterForUniqueResources(resourceCandidates);

        if (resourceLocations.empty())
            return;

        bool isServerStarting = !g_pGame->IsServerFullyUp();

        for (ResourceLocation& location : resourceLocations)
        {
            if (g_pServerInterface->IsRequestingExit())
                return;

            Resource* resource = GetResourceFromName(location.resourceName);

            if (resource != nullptr)
                continue;

            SArrayId scriptId = GenerateResourceScriptIdentifier();

            if (scriptId == INVALID_ARRAY_ID)
            {
                CLogger::LogPrintf("Loading of resource '%.*s' failed: unique script identifiers exhausted\n", location.resourceName.size(),
                                   location.resourceName.c_str());
                continue;
            }

            std::uint16_t remoteId = GenerateResourceRemoteIdentifier();

            if (remoteId == INVALID_RESOURCE_REMOTE_ID)
            {
                CLogger::LogPrintf("Loading of resource '%.*s' failed: unique remote identifiers exhausted\n", location.resourceName.size(),
                                   location.resourceName.c_str());
                continue;
            }

            if (location.type == ResourceLocationType::DIRECTORY)
            {
                resource = new Resource{*this};
                resource->SetSourceDirectory(location.absolutePath);
                resource->SetDynamicDirectory(location.absolutePath);
            }
            else if (location.type == ResourceLocationType::ZIP)
            {
                auto archiveResource = new ArchiveResource{*this};
                archiveResource->SetSourceDirectory(m_archiveDecompressionDirectory / location.resourceName);
                archiveResource->SetDynamicDirectory(fs::path{location.absolutePath}.replace_extension());
                archiveResource->SetSourceArchive(location.absolutePath);

                resource = archiveResource;
            }
            else
            {
                RecycleResourceScriptIdentifier(scriptId);
                RecycleResourceRemoteIdentifier(remoteId);
                continue;
            }

            resource->SetName(location.resourceName);
            resource->SetGroupDirectory(location.relativePath);
            resource->SetScriptIdentifier(scriptId);
            resource->SetRemoteIdentifier(remoteId);

            m_resources.push_back(resource);
            m_nameToResource[location.resourceName] = resource;
            m_scriptIdToResource[scriptId] = resource;
            m_remoteIdToResource[remoteId] = resource;

            if (resource->Load())
            {
                if (!isServerStarting)
                    CLogger::LogPrintf("New resource '%.*s' loaded\n", location.resourceName.size(), location.resourceName.c_str());

                ++m_numLoadedResources;
            }
            else
            {
                CLogger::LogPrintf("Loading of resource '%.*s' failed\n", location.resourceName.size(), location.resourceName.c_str());
                ++m_numErroneousResources;
            }
        }
    }

    void ResourceManager::DestroyZombieResource(Resource* resource)
    {
        const std::string& resourceName = resource->GetName();

        if (resource->IsRunning())
        {
            resource->SetWasDeleted();
            CLogger::ErrorPrintf("Resource '%.*s' has been removed while running! Stopping resource.\n", resourceName.size(), resourceName.c_str());
        }
        else
        {
            CLogger::LogPrintf("Resource '%.*s' has been removed\n", resourceName.size(), resourceName.c_str());
        }

        DestroyResource(resource);
    }

    void ResourceManager::ReloadChangedResource(Resource* resource)
    {
        const std::string& resourceName = resource->GetName();

        bool wasRunning = false;
        bool wasErroneous = false;

        if (resource->IsRunning())
        {
            wasRunning = true;

            CLogger::LogPrintf("Resource '%.*s' changed while running, reloading and restarting\n", resourceName.size(), resourceName.c_str());

            resource->Stop();
            resource->Unload();
        }
        else if (resource->GetState() == ResourceState::LOADED)
        {
            CLogger::LogPrintf("Resource '%.*s' changed, reloading\n", resourceName.size(), resourceName.c_str());

            resource->Unload();
        }
        else
        {
            wasErroneous = true;
        }

        if (resource->Load())
        {
            if (wasErroneous)
            {
                --m_numErroneousResources;
                ++m_numLoadedResources;
            }

            if (wasRunning)
            {
                resource->Start();
            }
        }
        else
        {
            if (!wasErroneous)
            {
                ++m_numErroneousResources;
                --m_numLoadedResources;
            }

            CLogger::LogPrintf("Loading of resource '%.*s' failed\n", resourceName.size(), resourceName.c_str());
        }
    }

    void ResourceManager::LoadErroneousResource(Resource* resource)
    {
        if (resource->Load())
        {
            --m_numErroneousResources;
            ++m_numLoadedResources;

            const std::string& resourceName = resource->GetName();
            CLogger::LogPrintf("New resource '%.*s' loaded\n", resourceName.size(), resourceName.c_str());
        }
    }

    void CreateDirectories(const fs::path& directory)
    {
        std::error_code ignore;
        fs::create_directories(directory, ignore);
    }

    void CreateResourceCacheReadme(const fs::path& directory)
    {
        fs::path filePath{directory / "DO_NOT_MODIFY_Readme.txt"};
        FILE*    fileHandle = File::Fopen(filePath.string().c_str(), "w");

        if (fileHandle == nullptr)
            return;

        fputs(R"(---------------------------------------------------------------------------
The content of this directory is automatically generated by the server.

Do not modify or delete anything in here while the server is running.

When the server is not running, you can do what you want, including clearing
out all the cached files by deleting the resource-cache directory.
(It will get recreated when the server is next started)
---------------------------------------------------------------------------

The 'http-client-files' directory always contains the correct client files
for hosting on a web server.
* If the web server is on the same machine, you can simply link the appropriate
  web server directory to 'http-client-files'.
* If the web server is on a separate machine, ensure it has access to
  'http-client-files' via a network path, or maintain a remote copy using
  synchronization software.
---------------------------------------------------------------------------
)", fileHandle);

        fclose(fileHandle);
    }

    ResourceNameToLocations LocateResources(const fs::path& resourcesDirectory)
    {
        std::error_code         ec;
        ResourceNameToLocations result;

        for (auto i = fs::recursive_directory_iterator{resourcesDirectory, ec}; i != fs::recursive_directory_iterator{} && !ec; i.increment(ec))
        {
            const fs::path& path = i->path();
            std::string     name = path.filename().generic_string();

            if (name.front() == '.')
            {
                i.disable_recursion_pending();
                continue;
            }

            if (i->is_directory(ec))
            {
                bool isResourceGroupDirectory = (name.front() == '#' || (name.front() == '[' && name.back() == ']'));

                if (isResourceGroupDirectory)
                    continue;

                i.disable_recursion_pending();

                if (fs::is_regular_file(path / "meta.xml", ec))
                {
                    ResourceLocation location;
                    location.type = ResourceLocationType::DIRECTORY;
                    location.absolutePath = path;
                    location.relativePath = path.lexically_relative(resourcesDirectory);
                    location.resourceName = name;

                    result[name].push_back(std::move(location));
                }
            }
            else if (i->is_regular_file(ec) && path.has_extension())
            {
                if (stricmp(".zip", path.extension().string().c_str()) != 0)
                    continue;

                name = path.stem().string();

                ResourceLocation location;
                location.type = ResourceLocationType::ZIP;
                location.absolutePath = path;
                location.relativePath = path.parent_path().lexically_relative(resourcesDirectory);
                location.resourceName = name;

                result[name].push_back(std::move(location));
            }
        }

        return result;
    }

    ResourceLocations FilterForUniqueResources(const ResourceNameToLocations& resourceCandidates)
    {
        ResourceLocations result;
        result.reserve(resourceCandidates.size());

        for (const auto& [name, sources] : resourceCandidates)
        {
            if (name.find_first_of(" .") != std::string::npos)
            {
                CLogger::LogPrintf("WARNING: Not loading resource '%.*s' as it contains illegal characters\n", name.size(), name.c_str());
                continue;
            }

            if (sources.size() == 1)
            {
                result.push_back(sources[0]);
            }
            else
            {
                CLogger::ErrorPrintf("Not processing resource '%.*s' as it has duplicates on different paths:\n", name.size(), name.c_str());

                for (std::size_t i = 0; i < sources.size(); i++)
                {
                    std::string relativePath = sources[i].relativePath.generic_string();
                    CLogger::LogPrintfNoStamp("                  Path #%zu: \"%.*s\"\n", i + 1, relativePath.size(), relativePath.c_str());
                }
            }
        }

        return result;
    }
}            // namespace mtasa
