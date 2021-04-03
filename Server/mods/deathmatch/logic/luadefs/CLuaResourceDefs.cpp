/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaResourceDefs.cpp
 *  PURPOSE:     Lua resource function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "Resource.h"
#include "ResourceFilePath.h"
#include "ResourceManager.h"
#include "SResourceStartOptions.h"
#include "CResourceConfigItem.h"
#include "../utils/CFunctionUseLogger.h"

using namespace mtasa;

extern CNetServer* g_pRealNetServer;

static Resource* ReadResourceArgument(CScriptArgReader& argStream, ResourceManager& resourceManager);

void CLuaResourceDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Create/edit functions
        {"createResource", createResource},
        {"copyResource", copyResource},
        {"renameResource", renameResource},
        {"deleteResource", deleteResource},

        {"addResourceMap", addResourceMap},
        {"addResourceConfig", addResourceConfig},
        {"removeResourceFile", removeResourceFile},

        {"setResourceDefaultSetting", setResourceDefaultSetting},
        {"removeResourceDefaultSetting", removeResourceDefaultSetting},

        // Start/stop management
        {"startResource", startResource},
        {"stopResource", stopResource},
        {"restartResource", restartResource},

        // Get stuff
        {"getThisResource", getThisResource},
        {"getResourceFromName", getResourceFromName},
        {"getResources", getResources},

        {"getResourceState", getResourceState},
        {"getResourceInfo", getResourceInfo},
        {"getResourceConfig", getResourceConfig},
        {"getResourceLoadFailureReason", getResourceLoadFailureReason},
        {"getResourceLastStartTime", getResourceLastStartTime},
        {"getResourceLoadTime", getResourceLoadTime},
        {"getResourceName", getResourceName},
        {"getResourceRootElement", getResourceRootElement},
        {"getResourceDynamicElementRoot", getResourceDynamicElementRoot},
        {"getResourceMapRootElement", getResourceMapRootElement},
        {"getResourceExportedFunctions", getResourceExportedFunctions},
        {"getResourceOrganizationalPath", getResourceOrganizationalPath},
        {"isResourceArchived", isResourceArchived},
        {"isResourceProtected", ArgumentParser<isResourceProtected>},

        // Set stuff
        {"setResourceInfo", setResourceInfo},

        // Misc
        {"call", call},
        {"refreshResources", refreshResources},

        {"getResourceACLRequests", getResourceACLRequests},
        {"loadstring", LoadString},
        {"load", Load},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);

    CLuaCFunctions::AddFunction("updateResourceACLRequest", updateResourceACLRequest, true);
}

void CLuaResourceDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    // These have been separated for their abnormal argument scheme
    // Their first arg take a path from which a resource is determined
    // For now, they are static-classes, and if the scheme is fixed
    // Then they will also be able to serve use when in an instance
    lua_classfunction(luaVM, "addConfig", "addResourceConfig");
    lua_classfunction(luaVM, "addMap", "addResourceMap");
    lua_classfunction(luaVM, "getConfig", "getResourceConfig");

    lua_classfunction(luaVM, "getFromName", "getResourceFromName");
    lua_classfunction(luaVM, "getAll", "getResources");
    lua_classfunction(luaVM, "getThis", "getThisResource");
    lua_classfunction(luaVM, "refresh", "refreshResources");            // Can't use "all" here because that's an argument

    lua_classfunction(luaVM, "create", "createResource");
    lua_classfunction(luaVM, "start", "startResource");
    lua_classfunction(luaVM, "stop", "stopResource");
    lua_classfunction(luaVM, "copy", "copyResource");
    lua_classfunction(luaVM, "rename", "renameResource");
    lua_classfunction(luaVM, "delete", "deleteResource");
    lua_classfunction(luaVM, "call", "call");
    lua_classfunction(luaVM, "removeDefaultSetting", "removeResourceDefaultSetting");
    lua_classfunction(luaVM, "removeFile", "removeResourceFile");
    lua_classfunction(luaVM, "restart", "restartResource");
    lua_classfunction(luaVM, "hasPermissionTo", "hasObjectPermissionTo");
    lua_classfunction(luaVM, "updateACLRequest", "updateResourceACLRequest");

    lua_classfunction(luaVM, "setInfo", "setResourceInfo");
    lua_classfunction(luaVM, "setDefaultSetting", "setResourceDefaultSetting");

    lua_classfunction(luaVM, "getDynamicElementRoot", "getResourceDynamicElementRoot");
    lua_classfunction(luaVM, "getRootElement", "getResourceRootElement");
    lua_classfunction(luaVM, "getExportedFunctions", "getResourceExportedFunctions");
    lua_classfunction(luaVM, "getOrganizationalPath", "getResourceOrganizationalPath");
    lua_classfunction(luaVM, "getLastStartTime", "getResourceLastStartTime");
    lua_classfunction(luaVM, "getLoadTime", "getResourceLoadTime");
    lua_classfunction(luaVM, "getInfo", "getResourceInfo");
    lua_classfunction(luaVM, "getLoadFailureReason", "getResourceLoadFailureReason");
    lua_classfunction(luaVM, "getMapRootElement", "getResourceMapRootElement");
    lua_classfunction(luaVM, "getName", "getResourceName");
    lua_classfunction(luaVM, "getState", "getResourceState");
    lua_classfunction(luaVM, "getACLRequests", "getResourceACLRequests");
    lua_classfunction(luaVM, "isArchived", "isResourceArchived");
    lua_classfunction(luaVM, "isProtected", "isResourceProtected");

    lua_classvariable(luaVM, "dynamicElementRoot", NULL, "getResourceDynamicElementRoot");
    lua_classvariable(luaVM, "exportedFunctions", NULL, "getResourceExportedFunctions");
    lua_classvariable(luaVM, "organizationalPath", nullptr, "getResourceOrganizationalPath");
    lua_classvariable(luaVM, "lastStartTime", NULL, "getResourceLastStartTime");
    lua_classvariable(luaVM, "aclRequests", NULL, "getResourceACLRequests");
    lua_classvariable(luaVM, "loadTime", NULL, "getResourceLoadTime");
    lua_classvariable(luaVM, "name", "renameResource", "getResourceName");
    lua_classvariable(luaVM, "rootElement", NULL, "getResourceRootElement");
    lua_classvariable(luaVM, "state", NULL, "getResourceState");
    lua_classvariable(luaVM, "archived", NULL, "isResourceArchived");
    lua_classvariable(luaVM, "protected", nullptr, "isResourceProtected");
    lua_classvariable(luaVM, "loadFailureReason", NULL, "getResourceLoadFailureReason");

    lua_registerclass(luaVM, "Resource");
}

int CLuaResourceDefs::createResource(lua_State* luaVM)
{
    //  resource createResource ( string toName[, string organizationalDir ] )
    std::string_view resourceName;
    std::string_view organizationPath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringView(resourceName);
    argStream.ReadStringView(organizationPath, true);

    if (!argStream.HasErrors())
    {
        Resource*           resource = nullptr;
        CreateResourceError error = m_resourceManager->TryCreateResource(resourceName, organizationPath, resource);

        if (error == CreateResourceError::NONE)
        {
            lua_pushresource(luaVM, resource);
            return 1;
        }

        switch (error)
        {
            // TODO:
            // argStream.SetCustomError(strStatus);
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::copyResource(lua_State* luaVM)
{
    //  resource copyResource ( mixed fromResource, string toName[, string organizationalDir ] )
    std::string_view newResourceName;
    std::string_view newGroupDirectory;

    CScriptArgReader argStream(luaVM);
    Resource*        resource = ReadResourceArgument(argStream, *m_resourceManager);
    argStream.ReadStringView(newResourceName);
    argStream.ReadStringView(newGroupDirectory, true);

    if (!argStream.HasErrors())
    {
        Resource*          newResource = nullptr;
        CloneResourceError error = m_resourceManager->TryCloneResource(resource, newResourceName, newGroupDirectory, newResource);

        if (error == CloneResourceError::NONE)
        {
            lua_pushresource(luaVM, newResource);
            return 1;
        }

        switch (error)
        {
            // TODO:
            // argStream.SetCustomError(strStatus);
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::renameResource(lua_State* luaVM)
{
    //  resource renameResource ( mixed fromResource, string newName[, string organizationalDir ] )
    std::string_view newResourceName;
    std::string_view newGroupDirectory;

    CScriptArgReader argStream(luaVM);
    Resource*        resource = ReadResourceArgument(argStream, *m_resourceManager);
    argStream.ReadStringView(newResourceName);
    argStream.ReadStringView(newGroupDirectory, true);

    if (!argStream.HasErrors())
    {
        RenameResourceError error = m_resourceManager->TryRenameResource(resource, newResourceName, newGroupDirectory);

        if (error == RenameResourceError::NONE)
        {
            lua_pushresource(luaVM, resource);
            return 1;
        }

        switch (error)
        {
            // TODO:
            // argStream.SetCustomError(strStatus);
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::deleteResource(lua_State* luaVM)
{
    //  bool deleteResource ( mixed resource )
    CScriptArgReader argStream(luaVM);
    Resource*        resource = ReadResourceArgument(argStream, *m_resourceManager);

    if (!argStream.HasErrors())
    {
        ResourceState resourceState = resource->GetState();

        if (resourceState == ResourceState::NOT_LOADED || resourceState == ResourceState::LOADED)
        {
            lua_pushboolean(luaVM, m_resourceManager->RemoveResource(resource));
            return 1;
        }
        else
        {
            const std::string& resourceName = resource->GetName();
            argStream.SetCustomError(SString{"Could not delete '%.*s' as the resource is running", resourceName.size(), resourceName.c_str()});
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::addResourceMap(lua_State* luaVM)
{
    if (lua_type(luaVM, 1) == LUA_TLIGHTUSERDATA)
        m_pScriptDebugging->LogCustom(luaVM, "addResourceMap may be using an outdated syntax. Please check and update.");

    std::string_view filePath;
    std::uint16_t    dimension;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringView(filePath);
    argStream.ReadNumber(dimension, 0);

    if (!argStream.HasErrors())
    {
        if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
        {
            std::optional<ResourceFilePath> file = ParseResourceFilePath(filePath, self);

            if (file.has_value())
            {
                CheckCanModifyOtherResource(argStream, self, file->resource);

                if (!argStream.HasErrors())
                {
                    CXMLNode* xmlNode = CStaticFunctionDefinitions::AddResourceMap(file->resource, file->absolutePath.generic_string(),
                                                                                   file->relativePath.generic_string(), dimension, self->GetLuaContext());

                    if (xmlNode != nullptr)
                    {
                        lua_pushxmlnode(luaVM, xmlNode);
                        return 1;
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::addResourceConfig(lua_State* luaVM)
{
    if (lua_type(luaVM, 1) == LUA_TLIGHTUSERDATA)
        m_pScriptDebugging->LogCustom(luaVM, "addResourceConfig may be using an outdated syntax. Please check and update.");

    std::string_view filePath;
    std::string_view configType;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringView(filePath);
    argStream.ReadStringView(configType, true);

    bool isServerSide = true;

    if (configType == "client"sv)
    {
        isServerSide = false;
    }
    else if (configType != "server"sv)
    {
        CLogger::LogPrintf("WARNING: Unknown config file type specified for %s. Defaulting to 'server'", lua_tostring(luaVM, lua_upvalueindex(1)));
    }

    if (!argStream.HasErrors())
    {
        if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
        {
            std::optional<ResourceFilePath> file = ParseResourceFilePath(filePath, self);

            if (file.has_value())
            {
                CheckCanModifyOtherResource(argStream, self, file->resource);

                if (!argStream.HasErrors())
                {
                    CXMLNode* xmlNode = CStaticFunctionDefinitions::AddResourceConfig(file->resource, file->absolutePath.generic_string(),
                                                                                        file->relativePath.generic_string(), isServerSide, self->GetLuaContext());

                    if (xmlNode != nullptr)
                    {
                        lua_pushxmlnode(luaVM, xmlNode);
                        return 1;
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::removeResourceFile(lua_State* luaVM)
{
    Resource*        resource = nullptr;
    std::string_view filePath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);
    argStream.ReadStringView(filePath);

    if (!argStream.HasErrors())
    {
        if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
        {
            CheckCanModifyOtherResource(argStream, self, resource);

            if (!argStream.HasErrors())
            {
                lua_pushboolean(luaVM, CStaticFunctionDefinitions::RemoveResourceFile(resource, filePath));
                return 1;
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::setResourceDefaultSetting(lua_State* luaVM)
{
    Resource* resource = nullptr;
    std::string_view name;
    std::string_view value;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);
    argStream.ReadStringView(name);
    argStream.ReadStringView(value);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, resource->SetDefaultSetting(name, value));
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
}

int CLuaResourceDefs::removeResourceDefaultSetting(lua_State* luaVM)
{
    Resource*   resource = nullptr;
    std::string settingName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);
    argStream.ReadString(settingName);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, resource->RemoveDefaultSetting(settingName));
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
}

int CLuaResourceDefs::startResource(lua_State* luaVM)
{
    Resource*             resource = nullptr;
    bool                  isPersistent = false;
    SResourceStartOptions StartOptions;

    // TODO: SResourceStartOptions

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);
    argStream.ReadBool(isPersistent, false);
    argStream.ReadBool(StartOptions.bIncludedResources, true);
    argStream.ReadBool(StartOptions.bConfigs, true);
    argStream.ReadBool(StartOptions.bMaps, true);
    argStream.ReadBool(StartOptions.bScripts, true);
    argStream.ReadBool(StartOptions.bHTML, true);
    argStream.ReadBool(StartOptions.bClientConfigs, true);
    argStream.ReadBool(StartOptions.bClientScripts, true);
    argStream.ReadBool(StartOptions.bClientFiles, true);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
        return 1;
    }

    const ResourceState resourceState = resource->GetState();
    const std::string& resourceName = resource->GetName();

    if (resourceState == ResourceState::NOT_LOADED)
    {
        const std::string& lastError = resource->GetLastError();
        m_pScriptDebugging->LogWarning(luaVM, "Failed to start resource '%.*s': %.*s", resourceName.size(), resourceName.c_str(), lastError.size(),
                                       lastError.c_str());
        lua_pushboolean(luaVM, false);
    }
    else if (resourceState == ResourceState::LOADED)
    {
        resource->SetPersistent(isPersistent);

        if (!m_resourceManager->StartResource(resource))
        {
            CLogger::LogPrintf("%s: Failed to start resource '%.*s'\n", lua_tostring(luaVM, lua_upvalueindex(1)), resourceName.size(), resourceName.c_str());
            lua_pushboolean(luaVM, false);
            return 1;
        }

        if (resource->IsRunning())
        {
            if (!isPersistent)
            {
                if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
                {
                    // TODO: Manually add ourselves to the starting resource's resource dependencies
                }
            }

            CLogger::LogPrintf("%s: Resource '%.*s' started\n", lua_tostring(luaVM, lua_upvalueindex(1)), resourceName.size(), resourceName.c_str());
            lua_pushboolean(luaVM, true);
        }
        else
        {
            m_pScriptDebugging->LogWarning(luaVM, "Failed to start resource '%.*s'", resourceName.size(), resourceName.c_str());
            lua_pushboolean(luaVM, false);
        }
    }
    else
    {
        lua_pushboolean(luaVM, false);
    }

    return 1;
}

int CLuaResourceDefs::stopResource(lua_State* luaVM)
{
    Resource* resource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
        return 1;
    }

    if (resource->IsStarting() || resource->IsRunning())
    {
        if (resource->IsProtected())
        {
            Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM);

            if (self == nullptr || !m_pACLManager->CanObjectUseRight(self->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                                    "stopResource.protected", CAccessControlListRight::RIGHT_TYPE_FUNCTION, false))
            {
                m_pScriptDebugging->LogError(luaVM, "%s: Resource could not be stopped as it is protected", lua_tostring(luaVM, lua_upvalueindex(1)));
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }

        m_resourceManager->QueueResourceCommand(resource, ResourceCommand::STOP);
        lua_pushboolean(luaVM, true);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::restartResource(lua_State* luaVM)
{
    Resource*             resource = nullptr;
    bool                  bPersistent = false;            // unused
    SResourceStartOptions StartOptions;

    // TODO: SResourceStartOptions

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);
    argStream.ReadBool(bPersistent, false);
    argStream.ReadBool(StartOptions.bConfigs, true);
    argStream.ReadBool(StartOptions.bMaps, true);
    argStream.ReadBool(StartOptions.bScripts, true);
    argStream.ReadBool(StartOptions.bHTML, true);
    argStream.ReadBool(StartOptions.bClientConfigs, true);
    argStream.ReadBool(StartOptions.bClientScripts, true);
    argStream.ReadBool(StartOptions.bClientFiles, true);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
        return 1;
    }

    if (resource->IsStarting() || resource->IsRunning())
    {
        m_resourceManager->QueueResourceCommand(resource, ResourceCommand::RESTART);
        lua_pushboolean(luaVM, true);
    }
    else
    {
        lua_pushboolean(luaVM, false);
    }

    return 1;
}

int CLuaResourceDefs::getThisResource(lua_State* luaVM)
{
    if (Resource* resource = m_pLuaManager->GetResourceFromLuaState(luaVM); resource != nullptr)
    {
        lua_pushresource(luaVM, resource);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceFromName(lua_State* luaVM)
{
    std::string_view resourceName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringView(resourceName);

    if (!argStream.HasErrors())
    {
        Resource* resource = m_resourceManager->GetResourceFromName(resourceName);

        if (resource != nullptr)
        {
            lua_pushresource(luaVM, resource);
            return 1;
        }
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResources(lua_State* luaVM)
{
    std::size_t index = 0;

    lua_newtable(luaVM);
    {
        for (Resource* resource : *m_resourceManager)
        {
            lua_pushnumber(luaVM, ++index);
            lua_pushresource(luaVM, resource);
            lua_settable(luaVM, -3);
        }
    }

    return 1;
}

int CLuaResourceDefs::getResourceState(lua_State* luaVM)
{
    Resource* resource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);

    if (!argStream.HasErrors())
    {
        switch (resource->GetState())
        {
            case ResourceState::LOADED:
                lua_pushliteral(luaVM, "loaded");
                break;
            case ResourceState::STARTING:
                lua_pushliteral(luaVM, "starting");
                break;
            case ResourceState::RUNNING:
                lua_pushliteral(luaVM, "running");
                break;
            case ResourceState::STOPPING:
                lua_pushliteral(luaVM, "stopping");
                break;
            case ResourceState::NOT_LOADED:
            default:
                lua_pushliteral(luaVM, "failed to load");
                break;
        }

        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
}

int CLuaResourceDefs::getResourceInfo(lua_State* luaVM)
{
    Resource*   resource = nullptr;
    std::string valueKey;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);
    argStream.ReadString(valueKey);

    if (!argStream.HasErrors())
    {
        std::string value;

        if (resource->TryGetInfoValue(valueKey, value))
        {
            lua_pushlstring(luaVM, value.c_str(), value.size());
            return 1;
        }

        lua_pushboolean(luaVM, false);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
}

int CLuaResourceDefs::setResourceInfo(lua_State* luaVM)
{
    Resource*   resource = nullptr;
    std::string name;
    std::string value;
    bool        removeAttribute = true;
    bool        persistChanges = true;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);
    argStream.ReadString(name);

    if (argStream.NextIsString())
    {
        argStream.ReadString(value);
        removeAttribute = false;
    }

    argStream.ReadBool(persistChanges, true);

    if (!argStream.HasErrors())
    {
        if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
        {
            CheckCanModifyOtherResource(argStream, self, resource);

            if (!argStream.HasErrors())
            {
                bool result = false;

                if (removeAttribute)
                    result = resource->RemoveInfoValue(name, persistChanges);
                else
                    result = resource->SetInfoValue(name, value, persistChanges);

                lua_pushboolean(luaVM, result);
                return 1;
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceConfig(lua_State* luaVM)
{
    std::string_view filePath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadStringView(filePath);

    if (!argStream.HasErrors())
    {
        if (Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM); self != nullptr)
        {
            std::optional<ResourceFilePath> file = ParseResourceFilePath(filePath, self);

            if (file.has_value())
            {
                CheckCanModifyOtherResource(argStream, self, file->resource);

                if (!argStream.HasErrors())
                {
                    CXMLNode* rootNode = file->resource->GetServerConfigFileRootNode(file->relativePath);

                    if (rootNode != nullptr)
                    {
                        lua_pushxmlnode(luaVM, rootNode);
                        return 1;
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceLoadFailureReason(lua_State* luaVM)
{
    Resource* resource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);

    if (!argStream.HasErrors())
    {
        const std::string& lastError = resource->GetLastError();
        lua_pushlstring(luaVM, lastError.c_str(), lastError.size());
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
}

int CLuaResourceDefs::getResourceLastStartTime(lua_State* luaVM)
{
    Resource* resource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);

    if (!argStream.HasErrors())
    {
        std::time_t time = resource->GetStartTime();

        if (time > 0)
            lua_pushnumber(luaVM, static_cast<lua_Number>(time));
        else
            lua_pushstring(luaVM, "never");

        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
}

int CLuaResourceDefs::getResourceLoadTime(lua_State* luaVM)
{
    Resource* resource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);

    if (!argStream.HasErrors())
    {
        std::time_t time = resource->GetLoadTime();

        if (time > 0)
            lua_pushnumber(luaVM, static_cast<lua_Number>(time));
        else
            lua_pushboolean(luaVM, false);

        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
}

int CLuaResourceDefs::getResourceName(lua_State* luaVM)
{
    Resource* resource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);

    if (!argStream.HasErrors())
    {
        const std::string& resourceName = resource->GetName();
        lua_pushlstring(luaVM, resourceName.c_str(), resourceName.size());
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
}

int CLuaResourceDefs::getResourceRootElement(lua_State* luaVM)
{
    Resource* resource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource, nullptr);

    if (!argStream.HasErrors())
    {
        if (resource == nullptr)
        {
            if (resource = m_pLuaManager->GetResourceFromLuaState(luaVM); resource == nullptr)
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }

        CElement* resourceRoot = resource->GetElement();

        if (resourceRoot != nullptr)
        {
            lua_pushelement(luaVM, resourceRoot);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
    return 1;
}

int CLuaResourceDefs::getResourceDynamicElementRoot(lua_State* luaVM)
{
    Resource* resource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource, nullptr);

    if (!argStream.HasErrors())
    {
        if (resource == nullptr)
        {
            if (resource = m_pLuaManager->GetResourceFromLuaState(luaVM); resource == nullptr)
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }

        CElement* dynamicElementRoot = resource->GetDynamicElementRoot();

        if (dynamicElementRoot != nullptr)
        {
            lua_pushelement(luaVM, dynamicElementRoot);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceMapRootElement(lua_State* luaVM)
{
    Resource*        resource = nullptr;
    std::string_view mapName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);
    argStream.ReadStringView(mapName);

    if (!argStream.HasErrors())
    {
        if (resource->IsRunning())
        {
            CElement* mapRoot = CStaticFunctionDefinitions::GetResourceMapRootElement(resource, mapName);

            if (mapRoot != nullptr)
            {
                lua_pushelement(luaVM, mapRoot);
                return 1;
            }
        }
        else
        {
            const std::string& resourceName = resource->GetName();

            m_pScriptDebugging->LogError(luaVM, "%s: Resource %.*s is not currently running", lua_tostring(luaVM, lua_upvalueindex(1)),
                                         resourceName.size(), resourceName.c_str());
        }
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceExportedFunctions(lua_State* luaVM)
{
    Resource* resource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource, nullptr);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }

    if (resource == nullptr)
        resource = m_pLuaManager->GetResourceFromLuaState(luaVM);

    if (resource == nullptr)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    lua_newtable(luaVM);
    {
        std::size_t index = 0;

        for (const std::string& functionName : resource->GetExportedServerFunctions())
        {
            lua_pushnumber(luaVM, ++index);
            lua_pushlstring(luaVM, functionName.c_str(), functionName.size());
            lua_settable(luaVM, -3);
        }
    }

    return 1;
}

int CLuaResourceDefs::getResourceOrganizationalPath(lua_State* luaVM)
{
    // string getResourceOrganizationalPath ( resource theResource )
    // Returns a string representing the resource organizational path, false if invalid resource was provided.

    Resource* resource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }

    lua_pushstring(luaVM, resource->GetGroupDirectory().generic_string().c_str());
    return 1;
}

int CLuaResourceDefs::call(lua_State* luaVM)
{
    Resource* self = m_pLuaManager->GetResourceFromLuaState(luaVM);

    if (self == nullptr)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    Resource*   resource = nullptr;
    std::string functionName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);
    argStream.ReadString(functionName);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }

    if (!resource->IsRunning())
    {
        const std::string& resourceName = resource->GetName();
        m_pScriptDebugging->LogError(luaVM, "%s: Failed, the resource %.*s isn't running", lua_tostring(luaVM, lua_upvalueindex(1)), resourceName.size(),
                                     resourceName.c_str());
        lua_pushboolean(luaVM, false);
        return 1;
    }

    // Is this an internal function that's restricted? To make sure you can't
    // call an MTA function in an external resource that's restricted and not
    // defined in ACL.
    bool           isRestricted = false;
    CLuaCFunction* luaFunction = CLuaCFunctions::GetFunction(functionName.c_str());

    if (luaFunction != nullptr)
        isRestricted = luaFunction->IsRestricted();

    // Check this resource can use the function call to the called resource
    if (!CanUseFunction(functionName.c_str(), self, isRestricted))
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    lua_State* functionLuaState = resource->GetLuaContext()->GetLuaState();
    LUA_CHECKSTACK(functionLuaState, 1);

    // Lets grab the original hidden variables so we can restore them later
    lua_getglobal(functionLuaState, "sourceResource");
    CLuaArgument OldResource(luaVM, -1);
    lua_pop(functionLuaState, 1);

    lua_getglobal(functionLuaState, "sourceResourceRoot");
    CLuaArgument OldResourceRoot(luaVM, -1);
    lua_pop(functionLuaState, 1);

    // Set the new values for the current sourceResource and sourceResourceRoot
    lua_pushresource(functionLuaState, self);
    lua_setglobal(functionLuaState, "sourceResource");

    lua_pushelement(functionLuaState, self->GetElement());
    lua_setglobal(functionLuaState, "sourceResourceRoot");

    CLuaArguments arguments;
    arguments.ReadArguments(luaVM, 3);

    CLuaArguments returns;

    bool success = resource->CallExportedFunction(functionName, arguments, returns, *self);

    // Restore the old variables
    OldResource.Push(functionLuaState);
    lua_setglobal(functionLuaState, "sourceResource");

    OldResourceRoot.Push(functionLuaState);
    lua_setglobal(functionLuaState, "sourceResourceRoot");

    if (success)
    {
        returns.PushArguments(luaVM);
        return returns.Count();
    }

    m_pScriptDebugging->LogError(luaVM, "%s: failed to call '%s:%s'", lua_tostring(luaVM, lua_upvalueindex(1)), resource->GetName().c_str(),
                                 functionName.c_str());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::refreshResources(lua_State* luaVM)
{
    //  bool refreshResources ( [ bool refreshAll = false, resource onlyThisResource = nil ] )
    bool      includeRunningResources = false;
    Resource* resource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(includeRunningResources, false);
    argStream.ReadUserData(resource, nullptr);

    if (!argStream.HasErrors())
    {
        if (resource != nullptr)
            m_resourceManager->QueueResourceCommand(resource, ResourceCommand::REFRESH);
        else
            m_resourceManager->QueueRefresh(includeRunningResources);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::getResourceACLRequests(lua_State* luaVM)
{
    //  table getResourceACLRequests ( resource theResource )
    Resource* pResource;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pResource);

    if (!argStream.HasErrors())
    {
        std::vector<SAclRequest> Result;
        pResource->GetAclRequests(Result);

        // Make table!
        lua_newtable(luaVM);
        for (uint i = 0; i < Result.size(); i++)
        {
            lua_newtable(luaVM);                     // new table
            lua_pushnumber(luaVM, i + 1);            // row index number (starting at 1, not 0)
            lua_pushvalue(luaVM, -2);                // value
            lua_settable(luaVM, -4);                 // refer to the top level table

            const SAclRequest& request = Result[i];
            lua_pushstring(luaVM, "name");
            lua_pushstring(luaVM, request.rightName.GetFullName());
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "access");
            lua_pushboolean(luaVM, request.bAccess);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "pending");
            lua_pushboolean(luaVM, request.bPending);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "who");
            lua_pushstring(luaVM, request.strWho);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "date");
            lua_pushstring(luaVM, request.strDate);
            lua_settable(luaVM, -3);

            lua_pop(luaVM, 1);            // pop the inner table
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::updateResourceACLRequest(lua_State* luaVM)
{
    //  bool updateResourceACLRequest ( resource theResource, string rightName, bool access, string byWho )
    Resource*        resource = nullptr;
    std::string_view right;
    bool             access;
    std::string      username;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);
    argStream.ReadStringView(right);
    argStream.ReadBool(access);
    argStream.ReadString(username, "");

    if (!argStream.HasErrors())
    {
        Resource* self = m_resourceManager->GetResourceFromLuaState(luaVM);

        if (username.empty() && self != nullptr)
            username = self->GetName();

        if (resource->HandleAclRequestChange(CAclRightName{right}, access, username))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::LoadString(lua_State* luaVM)
{
    Resource* resource = m_resourceManager->GetResourceFromLuaState(luaVM);

    if (resource == nullptr)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    //  func,err loadstring( string text[, string name] )
    SString strInput;
    SString strName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strInput);
    argStream.ReadString(strName, "");

    if (!argStream.HasErrors())
    {
        const char* szChunkname = strName.empty() ? *strInput : *strName;
        const char* cpInBuffer = strInput;
        uint        uiInSize = strInput.length();

        // Deobfuscate if required
        std::string scriptName = resource->GetName() + "/loadstring"s;
        const char* cpBuffer = nullptr;
        uint        uiSize = 0;

        if (!g_pRealNetServer->DeobfuscateScript(cpInBuffer, uiInSize, &cpBuffer, &uiSize, scriptName.c_str()))
        {
            SString strMessage("argument 1 is invalid. Please re-compile at http://luac.mtasa.com/", 0);
            argStream.SetCustomError(strMessage);
            cpBuffer = NULL;
        }

        if (!argStream.HasErrors())
        {
            CLuaShared::CheckUTF8BOMAndUpdate(&cpBuffer, &uiSize);
            if (!CLuaMain::LuaLoadBuffer(luaVM, cpBuffer, uiSize, szChunkname))
            {
                // Ok
                if (g_pGame->GetConfig()->GetLoadstringLogEnabled())
                    g_pGame->GetFunctionUseLogger()->OnFunctionUse(luaVM, "loadstring", cpBuffer, uiSize);
                return 1;
            }
            else
            {
                lua_pushnil(luaVM);
                lua_insert(luaVM, -2); /* put before error message */
                return 2;              /* return nil plus error message */
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::Load(lua_State* luaVM)
{
    Resource* resource = m_resourceManager->GetResourceFromLuaState(luaVM);

    if (resource == nullptr)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    //  func,err load( callback callbackFunction[, string name] )
    CLuaFunctionRef iLuaFunction;
    SString         strName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadFunction(iLuaFunction);
    argStream.ReadString(strName, "=(load)");
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        // Call supplied function to get all the bits
        // Should apply some limit here?
        SString       strInput;
        CLuaArguments callbackArguments;
        CLuaMain*     luaContext = m_pLuaManager->GetLuaContext(luaVM);

        while (luaContext)
        {
            CLuaArguments returnValues;
            callbackArguments.Call(luaContext, iLuaFunction, &returnValues);
            if (returnValues.Count())
            {
                CLuaArgument* returnedValue = *returnValues.IterBegin();
                int           iType = returnedValue->GetType();
                if (iType == LUA_TNIL)
                    break;

                else if (iType == LUA_TSTRING)
                {
                    std::string str = returnedValue->GetString();
                    if (str.length() == 0)
                        break;

                    strInput += str;
                    continue;
                }
            }
            break;
        }

        const char* szChunkname = *strName;
        const char* cpInBuffer = strInput;
        uint        uiInSize = strInput.length();

        // Deobfuscate if required
        std::string scriptName = resource->GetName() + "/load"s;
        const char* cpBuffer = nullptr;
        uint        uiSize = 0;

        if (!g_pRealNetServer->DeobfuscateScript(cpInBuffer, uiInSize, &cpBuffer, &uiSize, scriptName.c_str()))
        {
            SString strMessage("argument 2 is invalid. Please re-compile at http://luac.mtasa.com/", 0);
            argStream.SetCustomError(strMessage);
            cpBuffer = NULL;
        }

        if (!argStream.HasErrors())
        {
            CLuaShared::CheckUTF8BOMAndUpdate(&cpBuffer, &uiSize);
            if (!CLuaMain::LuaLoadBuffer(luaVM, cpBuffer, uiSize, szChunkname))
            {
                // Ok
                if (g_pGame->GetConfig()->GetLoadstringLogEnabled())
                    g_pGame->GetFunctionUseLogger()->OnFunctionUse(luaVM, "load", cpBuffer, uiSize);
                return 1;
            }
            else
            {
                lua_pushnil(luaVM);
                lua_insert(luaVM, -2); /* put before error message */
                return 2;              /* return nil plus error message */
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaResourceDefs::isResourceArchived(lua_State* luaVM)
{
    //  bool isResourceArchived ( resource theResource )
    Resource* resource = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(resource);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
        return 1;
    }

    lua_pushboolean(luaVM, resource->IsArchived());
    return 1;
}

bool CLuaResourceDefs::isResourceProtected(Resource* resource)
{
    return resource->IsProtected();
}

static Resource* ReadResourceArgument(CScriptArgReader& argStream, ResourceManager& resourceManager)
{
    Resource* resource = nullptr;

    if (argStream.NextIsString())
    {
        std::string_view resourceName;
        argStream.ReadStringView(resourceName);

        if (!argStream.HasErrors())
        {
            resource = resourceManager.GetResourceFromName(resourceName);

            if (resource == nullptr)
            {
                argStream.SetCustomError(SString{"Resource '%s' not found", resourceName.size(), resourceName.data()});
            }
        }
    }
    else
    {
        argStream.ReadUserData(resource);
    }

    return resource;
}
