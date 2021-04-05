/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource lifetime manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <string_view>
#include <deque>
#include <unordered_map>
#include <filesystem>
#include <LowercaseUtility.h>

constexpr std::uint16_t INVALID_RESOURCE_NET_ID = -1;

struct lua_State;

namespace mtasa
{
    using namespace std::string_view_literals;

    class Resource;
    class ResourceCommand;

    enum class CreateResourceError
    {
        NONE,
        DUMMY_FAIL,
    };

    enum class RenameResourceError
    {
        NONE,
        DUMMY_FAIL,
    };

    enum class CloneResourceError
    {
        NONE,
        DUMMY_FAIL,
    };

    class ResourceManager final
    {
        static constexpr auto RESOURCES_DIRECTORY_NAME = "resources"sv;
        static constexpr auto CACHE_DIRECTORY_NAME = "resource-cache"sv;
        static constexpr auto ARCHIVE_DECOMPRESSION_DIRECTORY_NAME = "unzipped"sv;

    public:
        ResourceManager(const std::filesystem::path& baseDirectory);
        ~ResourceManager();

        Resource* GetResourceFromName(std::string_view resourceName);
        Resource* GetResourceFromUniqueIdentifier(SArrayId identifier);
        Resource* GetResourceFromRemoteIdentifier(std::uint16_t identifier);

        CreateResourceError TryCreateResource(std::string_view resourceName, std::string_view relativeOrganizationPath, Resource*& newResource);
        RenameResourceError TryRenameResource(Resource* resource, std::string_view newResourceName, std::string_view newGroupDirectory);
        CloneResourceError  TryCloneResource(Resource* resource, std::string_view newResourceName, std::string_view newGroupDirectory, Resource*& newResource);

        void QueueResourceCommand(std::unique_ptr<ResourceCommand> command);
        
        void QueueRefresh(bool includeRunningResources)
        {
            m_isRefreshQueued = true;
            m_includeRunningResourcesRefresh = includeRunningResources;
        }

        void RefreshResources(bool includeRunningResources);
        void RefreshResource(std::string_view resourceName);
        void StopResources();

        void ProcessQueue();

        void ScanForResources();

        bool DeleteResource(Resource* resource);

        void OnPlayerJoin(CPlayer& player);

        const CMtaVersion& GetMinClientRequirement() const { return m_minClientRequirement; }

        std::size_t GetLoadedResourcesCount() const { return m_numLoadedResources; }

        // ???
        void        SaveBlockedFileReasons() {}
        void        ClearBlockedFileReasons() {}
        void        AddBlockedFileReason(std::string_view fileHash, std::string_view reason) {}
        std::string GetBlockedFileReason(const std::string& fileHash) { return ""; }

        std::vector<Resource*>::iterator begin() { return m_resources.begin(); }
        std::vector<Resource*>::iterator end() { return m_resources.end(); }

        std::vector<Resource*>::const_iterator begin() const { return m_resources.begin(); }
        std::vector<Resource*>::const_iterator end() const { return m_resources.end(); }

    private:
        SArrayId GenerateResourceUniqueIdentifier();
        void     RecycleResourceUniqueIdentifier(SArrayId id);

        std::uint16_t GenerateResourceRemoteIdentifier();
        void          RecycleResourceRemoteIdentifier(std::uint16_t id);

    private:
        std::filesystem::path m_resourcesDirectory;
        std::filesystem::path m_archiveDecompressionDirectory;

        std::unordered_map<std::string, Resource*, LowercaseHash, LowercaseEqual> m_nameToResource;

        std::unordered_map<SArrayId, Resource*>      m_uniqueIdToResource;
        std::unordered_map<std::uint16_t, Resource*> m_remoteIdToResource;

        std::vector<Resource*>     m_resources;
        std::vector<std::uint16_t> m_unusedResourceRemoteIdentifiers;

        std::size_t m_numLoadedResources = 0;
        std::size_t m_numErroneousResources = 0;

        CMtaVersion m_minClientRequirement;

        std::deque<std::unique_ptr<ResourceCommand>> m_commandsQueue;

        bool m_isRefreshQueued = false;
        bool m_includeRunningResourcesRefresh = false;
    };
}
