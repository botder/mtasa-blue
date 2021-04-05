/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Provide a resource from a directory
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "ResourceUseFlags.h"
#include <string>
#include <filesystem>
#include <optional>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <cstdint>

class CDummy;
class CXMLNode;
class CLuaMain;
class CElementGroup;

namespace mtasa
{
    class MetaFileParser;
    class ResourceManager;
    class ResourceFile;
    class ResourceHttpFile;
    class ClientResourceScriptFile;

    /* --> [NOT_LOADED] --(load)-->. [LOADED] --(start)--> [STARTING] .--(success)--> [RUNNING] .--(stop)--> [STOPPING] .
    *           |                  |  |    |                          |                         |                       |
    *           |`-----(failure)---'  |    |`----(failure/canceled)---'                         |                       |
    *           ^----------(unload)---'    |`--------------------------------------(canceled)---'                       |
    *                                      ^----------------------------------------------------------------------------'  */
    enum class ResourceState
    {
        NOT_LOADED,
        LOADED,
        STARTING,
        RUNNING,
        STOPPING,
    };

    class Resource
    {
    public:
        Resource(ResourceManager& resourceManager);

        virtual ~Resource();

        ResourceManager&       GetManager() { return m_resourceManager; }
        const ResourceManager& GetManager() const { return m_resourceManager; }

        void SetSourceDirectory(const std::filesystem::path& sourceDirectory)
        {
            m_sourceDirectory = sourceDirectory;
            m_metaFile = sourceDirectory / "meta.xml";
        }

        const std::filesystem::path& GetSourceDirectory() const { return m_sourceDirectory; }

        void SetDynamicDirectory(const std::filesystem::path& dynamicDirectory) { m_dynamicDirectory = dynamicDirectory; }

        void                         SetGroupDirectory(const std::filesystem::path& groupDirectory) { m_groupDirectory = groupDirectory; }
        const std::filesystem::path& GetGroupDirectory() const { return m_groupDirectory; }

        ResourceState GetState() const { return m_state; }
        bool          IsStarting() const { return m_state == ResourceState::STARTING; }
        bool          IsRunning() const { return m_state == ResourceState::RUNNING; }

        bool IsUsingOOP() const { return m_usingOOP; }

        int GetDownloadPriorityGroup() const { return m_downloadPriorityGroup; }

        const std::string& GetName() const { return m_name; }
        void               SetName(const std::string& name) { m_name = name; }

        const std::string& GetLastError() const { return m_lastError; }

        void     SetUniqueIdentifier(SArrayId id) { m_uniqueIdentifier = id; }
        SArrayId GetUniqueIdentifier() const { return m_uniqueIdentifier; }

        void          SetRemoteIdentifier(std::uint16_t id) { m_remoteIdentifier = id; }
        std::uint16_t GetRemoteIdentifier() const { return m_remoteIdentifier; }

        const CMtaVersion& GetMinServerVersion() const noexcept { return m_minServerVersion; }
        const CMtaVersion& GetMinClientVerison() const noexcept { return m_minClientVersion; }
        const CMtaVersion& GetMetaMinServerVersion() const noexcept { return m_metaMinServerVersion; }
        const CMtaVersion& GetMetaMinClientVerison() const noexcept { return m_metaMinClientVersion; }

        CDummy*       GetElement() { return m_element; }
        const CDummy* GetElement() const { return m_element; }

        CDummy*       GetDynamicElementRoot() { return m_dynamicElementRoot; }
        const CDummy* GetDynamicElementRoot() const { return m_dynamicElementRoot; }

        CElementGroup*       GetElementGroup() { return m_elementGroup; }
        const CElementGroup* GetElementGroup() const { return m_elementGroup; }

        CLuaMain*       GetLuaContext() { return m_luaContext; }
        const CLuaMain* GetLuaContext() const { return m_luaContext; }

        bool IsClientSynced() const { return m_syncElementsToClients; }

        void SetProtected(bool isProtected) { m_isProtected = isProtected; }
        bool IsProtected() const { return m_isProtected; }

        void SetPersistent(bool isPersistent) { m_isPersistent = isPersistent; }
        bool IsPersistent() const { return m_isPersistent; }

        virtual bool IsArchived() const { return false; }

        virtual bool Unload();
        virtual bool Load();
        virtual bool Start(ResourceUseFlags useFlags = {});
        virtual bool Stop();

        bool Restart(ResourceUseFlags useFlags = {}) { return Stop() && Start(useFlags); }

        // This function doesn't verify the absolute file path against resource directory escape attacks
        std::filesystem::path GetUnsafeAbsoluteFilePath(const std::filesystem::path& relativePath);
        
        bool CallExportedFunction(const std::string& functionName, CLuaArguments& arguments, CLuaArguments& returnValues, Resource& sourceResource);

        std::vector<std::string_view> GetExportedServerFunctions() const;
        std::vector<std::string_view> GetExportedClientFunctions() const;

        std::time_t GetStartTime() const { return m_startedTime; }
        std::time_t GetLoadTime() const { return m_loadedTime; }

        bool SetInfoValue(const std::string& key, const std::string& value, bool persistChanges);
        bool TryGetInfoValue(const std::string& key, std::string& value) const;
        bool RemoveInfoValue(const std::string& key, bool persistChanges);

        CXMLNode* GetServerConfigFileRootNode(const std::filesystem::path& relativePath) const { return nullptr; }

        std::size_t GetNoCacheClientScriptsCount() const { return m_noCacheClientScripts.size(); }

        // Remove a resource default setting
        bool RemoveDefaultSetting(const std::string& settingName) { return false; }

        // Set a resource default setting
        bool SetDefaultSetting(std::string_view name, std::string_view value) { return false; }

        std::size_t GetDependentCount() const { return 0; }
        std::size_t GetFileCount() const { return 0; }

        // ??????????
        bool CheckFunctionRightCache(lua_CFunction f, bool* pbOutAllowed) { return false; }
        void UpdateFunctionRightCache(lua_CFunction f, bool bAllowed) {}
        void GetAclRequests(std::vector<SAclRequest>& outResultList) {}
        bool HandleAclRequestListCommand(bool bDetail) { return true; }
        bool HandleAclRequestChangeCommand(const SString& strRightName, bool bAccess, const SString& strWho) { return true; }
        bool HandleAclRequestChange(const CAclRightName& strRightName, bool bAccess, const SString& strWho) { return true; }

    protected:
        virtual bool ContainsSourceFile(const std::filesystem::path& relativePath) const;

        ResourceManager&      m_resourceManager;
        ResourceState         m_state = ResourceState::NOT_LOADED;
        std::string           m_name;
        std::string           m_lastError;
        std::filesystem::path m_groupDirectory;
        std::filesystem::path m_sourceDirectory;
        std::filesystem::path m_dynamicDirectory;
        std::filesystem::path m_metaFile;

        bool m_syncElementsToClients = false;
        bool m_isProtected = false;
        bool m_isPersistent = false;

        ResourceUseFlags m_useFlags;

        SArrayId      m_uniqueIdentifier = INVALID_ARRAY_ID;
        std::uint16_t m_remoteIdentifier = 0xFFFF;

        std::unordered_map<std::string, std::string> m_info;

        bool m_usingOOP = false;
        int  m_downloadPriorityGroup = 0;

        std::vector<std::filesystem::path>                         m_serverFilePaths;
        std::vector<std::pair<std::filesystem::path, std::string>> m_clientFilePaths;

        CLuaMain* m_luaContext = nullptr;

        CDummy* m_mapRootElement = nullptr;
        CDummy* m_element = nullptr;
        CDummy* m_dynamicElementRoot = nullptr;

        CElementGroup* m_elementGroup = nullptr;

        CXMLNode* m_settingsNode = nullptr;
        CXMLNode* m_tempSettingsNode = nullptr;

        std::time_t m_loadedTime = 0;
        std::time_t m_startedTime = 0;

        CMtaVersion m_minClientVersion;
        CMtaVersion m_minServerVersion;
        CMtaVersion m_metaMinClientVersion;
        CMtaVersion m_metaMinServerVersion;

    protected:
        struct DependencyVersion
        {
            std::uint32_t major = 0;
            std::uint32_t minor = 0;
            std::uint32_t revision = 0;
        };

        struct Dependency
        {
            std::string       resourceName;
            DependencyVersion minVersion;
            DependencyVersion maxVersion;
        };
        std::vector<Dependency> m_dependencies;

    protected:
        struct ClientFunction
        {
            std::string  name;
            std::uint8_t isHttpAccessible : 1;
            std::uint8_t isACLRestricted : 1;

            ClientFunction() : isHttpAccessible{false}, isACLRestricted{false} {}
        };
        std::vector<ClientFunction> m_clientFunctions;

        struct ServerFunction
        {
            std::uint8_t isHttpAccessible : 1;
            std::uint8_t isACLRestricted : 1;

            ServerFunction() : isHttpAccessible{false}, isACLRestricted{false} {}
        };
        std::unordered_map<std::string, ServerFunction> m_serverFunctions;

    protected:
        std::vector<std::unique_ptr<ResourceFile>> m_resourceFiles;
        std::vector<ResourceHttpFile*>             m_httpFiles;
        std::vector<ClientResourceScriptFile*>     m_noCacheClientScripts;
        std::vector<ResourceFile*>                 m_clientFiles;

    private:
        bool PreProcessResourceFiles();

        bool CreateLuaContext();
        void DeleteLuaContext();

        bool IsDuplicateServerFile(const std::filesystem::path& relativeFilePath);
        bool IsDuplicateClientFile(const std::filesystem::path& relativeFilePath);

        void AddServerFilePath(const std::filesystem::path& relativeFilePath);
        void AddClientFilePath(const std::filesystem::path& relativeFilePath);

        bool ProcessMeta(const MetaFileParser& meta);
        bool ProcessMetaIncludes(const MetaFileParser& meta);
        bool ProcessMetaMaps(const MetaFileParser& meta);
        bool ProcessMetaFiles(const MetaFileParser& meta);
        bool ProcessMetaScripts(const MetaFileParser& meta);
        bool ProcessMetaHttpFiles(const MetaFileParser& meta);
        bool ProcessMetaExports(const MetaFileParser& meta);
        bool ProcessMetaConfigs(const MetaFileParser& meta);
    };
}            // namespace mtasa
