/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaManager.cpp
 *  PURPOSE:     Lua virtual machine manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "../luadefs/CLuaGenericDefs.h"

using namespace mtasa;

extern CGame* g_pGame;

CLuaManager::CLuaManager(CObjectManager* pObjectManager, CPlayerManager* pPlayerManager, CVehicleManager* pVehicleManager, CBlipManager* pBlipManager,
                         CRadarAreaManager* pRadarAreaManager, CRegisteredCommands* pRegisteredCommands, CMapManager* pMapManager, CEvents* pEvents)
{
    m_pObjectManager = pObjectManager;
    m_pPlayerManager = pPlayerManager;
    m_pVehicleManager = pVehicleManager;
    m_pBlipManager = pBlipManager;
    m_pRadarAreaManager = pRadarAreaManager;
    m_pRegisteredCommands = pRegisteredCommands;
    m_pMapManager = pMapManager;
    m_pEvents = pEvents;

    // Create our lua dynamic module manager
    m_pLuaModuleManager = new CLuaModuleManager(this);
    m_pLuaModuleManager->SetScriptDebugging(g_pGame->GetScriptDebugging());

    // Load our C Functions into Lua and hook callback
    LoadCFunctions();
    lua_registerPreCallHook(CLuaDefs::CanUseFunction);
    lua_registerUndumpHook(CLuaMain::OnUndump);

#ifdef MTA_DEBUG
    // Check rounding in case json is updated
    json_object* obj = json_object_new_double(5.1);
    SString      strResult = json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PLAIN);
    assert(strResult == "5.1");
    json_object_put(obj);
#endif
}

CLuaManager::~CLuaManager()
{
    CLuaCFunctions::RemoveAllFunctions();

    for (CLuaMain* luaContext : std::exchange(m_luaContexts, {}))
    {
        delete luaContext;
    }

    delete m_pLuaModuleManager;
}

CLuaMain* CLuaManager::CreateLuaContext(Resource& ownerResource, bool bEnableOOP)
{
    auto luaContext = new CLuaMain(ownerResource, this, m_pObjectManager, m_pPlayerManager, m_pVehicleManager, m_pBlipManager, m_pRadarAreaManager,
                                   m_pMapManager, bEnableOOP);

    m_luaContexts.push_back(luaContext);

    luaContext->InitVM();

    m_pLuaModuleManager->RegisterFunctions(luaContext->GetLuaState());

    return luaContext;
}

bool CLuaManager::RemoveLuaContext(CLuaMain* luaContext)
{
    if (luaContext)
    {
        // Remove all events registered by it and all commands added
        m_pEvents->RemoveAllEvents(luaContext);
        m_pRegisteredCommands->CleanUpForVM(luaContext);

        // Delete it unless it is already
        if (!luaContext->BeingDeleted())
        {
            delete luaContext;
        }

        m_luaContexts.erase(std::remove(m_luaContexts.begin(), m_luaContexts.end(), luaContext));
        return true;
    }

    return false;
}

void CLuaManager::OnLuaStateOpen(CLuaMain* luaContext, lua_State* luaVM)
{
    MapSet(m_luaStateToLuaContext, luaContext->GetLuaState(), luaContext);
}

void CLuaManager::OnLuaStateClose(CLuaMain* luaContext, lua_State* luaVM)
{
    MapRemove(m_luaStateToLuaContext, luaContext->GetLuaState());
}

void CLuaManager::DoPulse()
{
    for (CLuaMain* luaContext : m_luaContexts)
        luaContext->DoPulse();

    m_pLuaModuleManager->DoPulse();
}

CLuaMain* CLuaManager::GetLuaContext(lua_State* luaState)
{
    if (luaState == nullptr)
        return nullptr;

    // Grab the main virtual state because the one we've got passed might be a coroutine state
    // and only the main state is in our list.
    if (lua_State* mainLuaState = lua_getmainstate(luaState); mainLuaState != nullptr)
        luaState = mainLuaState;

    // Find a matching Lua context in our map
    if (CLuaMain* luaContext = MapFindRef(m_luaStateToLuaContext, luaState); luaContext != nullptr)
        return luaContext;

    // Find a matching Lua context in our list
    auto iter =
        std::find_if(m_luaContexts.begin(), m_luaContexts.end(), [luaState](const CLuaMain* luaContext) { return luaContext->GetLuaState() == luaState; });

    if (iter != m_luaContexts.end())
    {
        dassert(0);             // Why not in map?
        return *iter;
    }

    return nullptr;
}

// Return resource associated with a lua state
Resource* CLuaManager::GetResourceFromLuaState(lua_State* luaVM)
{
    if (CLuaMain* luaContext = GetLuaContext(luaVM); luaContext != nullptr)
        return &luaContext->GetResource();

    return nullptr;
}

void CLuaManager::LoadCFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"addEvent", CLuaFunctionDefs::AddEvent},
        {"addEventHandler", CLuaFunctionDefs::AddEventHandler},
        {"removeEventHandler", CLuaFunctionDefs::RemoveEventHandler},
        {"getEventHandlers", CLuaFunctionDefs::GetEventHandlers},
        {"triggerEvent", CLuaFunctionDefs::TriggerEvent},
        {"triggerClientEvent", CLuaFunctionDefs::TriggerClientEvent},
        {"cancelEvent", CLuaFunctionDefs::CancelEvent},
        {"wasEventCancelled", CLuaFunctionDefs::WasEventCancelled},
        {"getCancelReason", CLuaFunctionDefs::GetCancelReason},
        {"triggerLatentClientEvent", CLuaFunctionDefs::TriggerLatentClientEvent},
        {"getLatentEventHandles", CLuaFunctionDefs::GetLatentEventHandles},
        {"getLatentEventStatus", CLuaFunctionDefs::GetLatentEventStatus},
        {"cancelLatentEvent", CLuaFunctionDefs::CancelLatentEvent},
        {"addDebugHook", CLuaFunctionDefs::AddDebugHook},
        {"removeDebugHook", CLuaFunctionDefs::RemoveDebugHook},

        // Explosion create funcs
        {"createExplosion", CLuaFunctionDefs::CreateExplosion},

        // Fire create funcs
        // CLuaCFunctions::AddFunction ( "createFire", CLuaFunctionDefinitions::CreateFire );

        // Path(node) funcs
        // CLuaCFunctions::AddFunction ( "createNode", CLuaFunctionDefinitions::CreateNode );

        // Ped body funcs?
        {"getBodyPartName", CLuaFunctionDefs::GetBodyPartName},
        {"getClothesByTypeIndex", CLuaFunctionDefs::GetClothesByTypeIndex},
        {"getTypeIndexFromClothes", CLuaFunctionDefs::GetTypeIndexFromClothes},
        {"getClothesTypeName", CLuaFunctionDefs::GetClothesTypeName},

        // Weapon funcs
        {"getWeaponNameFromID", CLuaFunctionDefs::GetWeaponNameFromID},
        {"getWeaponIDFromName", CLuaFunctionDefs::GetWeaponIDFromName},
        {"getWeaponProperty", CLuaFunctionDefs::GetWeaponProperty},
        {"getOriginalWeaponProperty", CLuaFunctionDefs::GetOriginalWeaponProperty},
        {"setWeaponProperty", CLuaFunctionDefs::SetWeaponProperty},
        {"setWeaponAmmo", CLuaFunctionDefs::SetWeaponAmmo},
        {"getSlotFromWeapon", CLuaFunctionDefs::GetSlotFromWeapon},

    #if MTASA_VERSION_TYPE < VERSION_TYPE_RELEASE
        {"createWeapon", CLuaFunctionDefs::CreateWeapon},
        {"fireWeapon", CLuaFunctionDefs::FireWeapon},
        {"setWeaponState", CLuaFunctionDefs::SetWeaponState},
        {"getWeaponState", CLuaFunctionDefs::GetWeaponState},
        {"setWeaponTarget", CLuaFunctionDefs::SetWeaponTarget},
        {"getWeaponTarget", CLuaFunctionDefs::GetWeaponTarget},
        {"setWeaponOwner", CLuaFunctionDefs::SetWeaponOwner},
        {"getWeaponOwner", CLuaFunctionDefs::GetWeaponOwner},
        {"setWeaponFlags", CLuaFunctionDefs::SetWeaponFlags},
        {"getWeaponFlags", CLuaFunctionDefs::GetWeaponFlags},
        {"setWeaponFiringRate", CLuaFunctionDefs::SetWeaponFiringRate},
        {"getWeaponFiringRate", CLuaFunctionDefs::GetWeaponFiringRate},
        {"resetWeaponFiringRate", CLuaFunctionDefs::ResetWeaponFiringRate},
        {"getWeaponAmmo", CLuaFunctionDefs::GetWeaponAmmo},
        {"getWeaponClipAmmo", CLuaFunctionDefs::GetWeaponClipAmmo},
        {"setWeaponClipAmmo", CLuaFunctionDefs::SetWeaponClipAmmo},
    #endif

        // Console funcs
        {"addCommandHandler", CLuaFunctionDefs::AddCommandHandler},
        {"removeCommandHandler", CLuaFunctionDefs::RemoveCommandHandler},
        {"executeCommandHandler", CLuaFunctionDefs::ExecuteCommandHandler},
        {"getCommandHandlers", CLuaFunctionDefs::GetCommandHandlers},

        // Server standard funcs
        {"getMaxPlayers", CLuaFunctionDefs::GetMaxPlayers},
        {"setMaxPlayers", CLuaFunctionDefs::SetMaxPlayers},
        {"outputChatBox", CLuaFunctionDefs::OutputChatBox},
        {"outputConsole", CLuaFunctionDefs::OutputConsole},
        {"outputDebugString", CLuaFunctionDefs::OutputDebugString},
        {"outputServerLog", CLuaFunctionDefs::OutputServerLog},
        {"getServerName", CLuaFunctionDefs::GetServerName},
        {"getServerHttpPort", CLuaFunctionDefs::GetServerHttpPort},
        {"getServerPassword", CLuaFunctionDefs::GetServerPassword},
        {"setServerPassword", CLuaFunctionDefs::SetServerPassword},
        {"getServerConfigSetting", CLuaFunctionDefs::GetServerConfigSetting},
        {"clearChatBox", CLuaFunctionDefs::ClearChatBox},

        // Loaded map funcs
        {"getRootElement", CLuaFunctionDefs::GetRootElement},
        {"loadMapData", CLuaFunctionDefs::LoadMapData},
        {"saveMapData", CLuaFunctionDefs::SaveMapData},

        // All-Seeing Eye Functions
        {"getGameType", CLuaFunctionDefs::GetGameType},
        {"getMapName", CLuaFunctionDefs::GetMapName},
        {"setGameType", CLuaFunctionDefs::SetGameType},
        {"setMapName", CLuaFunctionDefs::SetMapName},
        {"getRuleValue", CLuaFunctionDefs::GetRuleValue},
        {"setRuleValue", CLuaFunctionDefs::SetRuleValue},
        {"removeRuleValue", CLuaFunctionDefs::RemoveRuleValue},

        // Registry functions
        {"getPerformanceStats", CLuaFunctionDefs::GetPerformanceStats},

         // Admin functions
        /*
        CLuaCFunctions::AddFunction ( "aexec", CLuaFunctionDefinitions::Aexec },
        CLuaCFunctions::AddFunction ( "kickPlayer", CLuaFunctionDefinitions::KickPlayer },
        CLuaCFunctions::AddFunction ( "banPlayer", CLuaFunctionDefinitions::BanPlayer },
        CLuaCFunctions::AddFunction ( "banPlayerIP", CLuaFunctionDefinitions::BanPlayerIP },
        CLuaCFunctions::AddFunction ( "setPlayerMuted", CLuaFunctionDefinitions::SetPlayerMuted },

        CLuaCFunctions::AddFunction ( "addAccount", CLuaFunctionDefinitions::AddAccount },
        CLuaCFunctions::AddFunction ( "delAccount", CLuaFunctionDefinitions::DelAccount },
        CLuaCFunctions::AddFunction ( "setAccountPassword", CLuaFunctionDefinitions::SetAccountPassword },
        */

        // Misc funcs
        {"resetMapInfo", CLuaFunctionDefs::ResetMapInfo},
        {"getServerPort", CLuaFunctionDefs::GetServerPort},

        // Settings registry funcs
        {"get", CLuaFunctionDefs::Get},
        {"set", CLuaFunctionDefs::Set},

        // Utility
        {"getVersion", CLuaFunctionDefs::GetVersion},
        {"getNetworkUsageData", CLuaFunctionDefs::GetNetworkUsageData},
        {"getNetworkStats", CLuaFunctionDefs::GetNetworkStats},
        {"getLoadedModules", CLuaFunctionDefs::GetModules},
        {"getModuleInfo", CLuaFunctionDefs::GetModuleInfo},

        {"setDevelopmentMode", CLuaFunctionDefs::SetDevelopmentMode},
        {"getDevelopmentMode", CLuaFunctionDefs::GetDevelopmentMode},
    };

    // Add all functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);

    // Restricted functions
    CLuaCFunctions::AddFunction("setServerConfigSetting", CLuaFunctionDefs::SetServerConfigSetting, true);
    CLuaCFunctions::AddFunction("shutdown", CLuaFunctionDefs::shutdown, true);

    // Load the functions from our classes
    CLuaACLDefs::LoadFunctions();
    CLuaAccountDefs::LoadFunctions();
    CLuaBanDefs::LoadFunctions();
    CLuaBlipDefs::LoadFunctions();
    CLuaCameraDefs::LoadFunctions();
    CLuaColShapeDefs::LoadFunctions();
    CLuaDatabaseDefs::LoadFunctions();
    CLuaElementDefs::LoadFunctions();
    CLuaHandlingDefs::LoadFunctions();
    CLuaMarkerDefs::LoadFunctions();
    CLuaNetworkDefs::LoadFunctions();
    CLuaObjectDefs::LoadFunctions();
    CLuaPedDefs::LoadFunctions();
    CLuaPickupDefs::LoadFunctions();
    CLuaPlayerDefs::LoadFunctions();
    CLuaRadarAreaDefs::LoadFunctions();
    CLuaResourceDefs::LoadFunctions();
    CLuaShared::LoadFunctions();
    CLuaTeamDefs::LoadFunctions();
    CLuaTextDefs::LoadFunctions();
    CLuaTimerDefs::LoadFunctions();
    CLuaVehicleDefs::LoadFunctions();
    CLuaVoiceDefs::LoadFunctions();
    CLuaWaterDefs::LoadFunctions();
    CLuaWorldDefs::LoadFunctions();
    CLuaXMLDefs::LoadFunctions();
    CLuaGenericDefs::LoadFunctions();
    // Backward compatibility functions at the end, so the new function name is used in ACL
    CLuaCompatibilityDefs::LoadFunctions();
}
