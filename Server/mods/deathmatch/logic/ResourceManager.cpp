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

    ResourceManager::~ResourceManager()
    {
        for (Resource* resource : std::exchange(m_resources, {}))
        {
            resource->Stop();
            resource->Unload();
            OnResourceDelete(resource);
            RecycleResourceScriptIdentifier(resource->GetScriptIdentifier());
            delete resource;
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

    CreateResourceError ResourceManager::TryCreateResource(std::string_view resourceName, std::string_view relativeOrganizationPath, Resource*& newResource)
    {
        return CreateResourceError::DUMMY_FAIL;
    }

    RenameResourceError ResourceManager::TryRenameResource(Resource* resource, std::string_view newResourceName, std::string_view newGroupDirectory)
    {
        return RenameResourceError::DUMMY_FAIL;
    }

    CloneResourceError ResourceManager::TryCloneResource(Resource* resource, std::string_view newResourceName, std::string_view newGroupDirectory,
                                                         Resource*& newResource)
    {
        return CloneResourceError::DUMMY_FAIL;
    }

    void ResourceManager::QueueResourceCommand(std::unique_ptr<ResourceCommand> command)
    {
        m_commandsQueue.push_back(std::move(command));
    }

    void ResourceManager::RefreshResources(bool includeRunningResources)
    {
        // TODO: Add implementation here
    }

    void ResourceManager::RefreshResource(std::string_view resourceName)
    {
        // TODO: Add implementation here
    }

    void ResourceManager::RefreshResource(Resource* resource)
    {
        // TODO: Add implementation here
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

    void ResourceManager::ScanForResources()
    {
        ResourceNameToLocations resourceCandidates = LocateResources(m_resourcesDirectory);
        ResourceLocations       resourceLocations = FilterForUniqueResources(resourceCandidates);

        for (ResourceLocation& location : resourceLocations)
        {
            SArrayId scriptId = GenerateResourceScriptIdentifier();

            if (scriptId == INVALID_ARRAY_ID)
            {
                CLogger::LogPrintf("Loading of resource '%.*s' failed: unique script identifiers exhausted\n", location.resourceName.size(),
                                   location.resourceName.c_str());
                break;
            }

            std::uint16_t remoteId = GenerateResourceRemoteIdentifier();

            if (remoteId == INVALID_RESOURCE_REMOTE_ID)
            {
                CLogger::LogPrintf("Loading of resource '%.*s' failed: unique remote identifiers exhausted\n", location.resourceName.size(),
                                   location.resourceName.c_str());
                break;
            }

            Resource* resource = nullptr;

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
                continue;
            }

            resource->SetName(location.resourceName);
            resource->SetGroupDirectory(location.relativePath);
            resource->SetScriptIdentifier(scriptId);
            resource->SetRemoteIdentifier(remoteId);

            OnResourceCreate(resource);

            if (!resource->Load())
            {
                CLogger::LogPrintf("Loading of resource '%.*s' (sid: %lx, rid: %u) failed\n", location.resourceName.size(), location.resourceName.c_str(),
                                   scriptId, remoteId);
                m_numErroneousResources++;
            }
            else
            {
                m_numLoadedResources++;
            }
        }
    }

    bool ResourceManager::DeleteResource(Resource* resource)
    {
        // TODO: Add implementation here
        return false;
    }

    void ResourceManager::OnPlayerJoin(CPlayer& player)
    {
        // TODO: Add implementation here
    }

    SArrayId ResourceManager::GenerateResourceScriptIdentifier()
    {
        return CIdArray::PopUniqueId(this, EIdClass::RESOURCE); }

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
        m_unusedResourceRemoteIdentifiers.push_back(id); }

    void ResourceManager::OnResourceCreate(Resource* resource)
    {
        m_resources.push_back(resource);
        m_nameToResource[resource->GetName()] = resource;
        m_scriptIdToResource[resource->GetScriptIdentifier()] = resource;
        m_remoteIdToResource[resource->GetRemoteIdentifier()] = resource;
    }

    void ResourceManager::OnResourceDelete(Resource* resource)
    {
        m_remoteIdToResource.erase(resource->GetRemoteIdentifier());
        m_scriptIdToResource.erase(resource->GetScriptIdentifier());
        m_nameToResource.erase(resource->GetName());

        if (!m_resources.empty())
            m_resources.erase(std::remove(m_resources.begin(), m_resources.end(), resource));
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
