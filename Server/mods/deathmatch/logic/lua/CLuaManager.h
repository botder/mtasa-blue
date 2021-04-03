/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaManager.h
 *  PURPOSE:     Lua virtual machine manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <vector>

class CBlipManager;
class CEvents;
class CMapManager;
class CObjectManager;
class CPlayerManager;
class CRadarAreaManager;
class CRegisteredCommands;
class CVehicleManager;
class CLuaModuleManager;

namespace mtasa
{
    class Resource;
}

class CLuaManager
{
public:
    CLuaManager(CObjectManager* pObjectManager, CPlayerManager* pPlayerManager, CVehicleManager* pVehicleManager, CBlipManager* pBlipManager,
                CRadarAreaManager* pRadarAreaManager, CRegisteredCommands* pRegisteredCommands, CMapManager* pMapManager, CEvents* pEvents);
    ~CLuaManager();

    CLuaMain*        CreateLuaContext(mtasa::Resource& ownerResource, bool bEnableOOP);
    bool             RemoveLuaContext(CLuaMain* luaContext);
    CLuaMain*        GetLuaContext(lua_State* luaState);
    mtasa::Resource* GetResourceFromLuaState(lua_State* luaVM);
    void             OnLuaStateOpen(CLuaMain* luaContext, lua_State* luaVM);
    void             OnLuaStateClose(CLuaMain* luaContext, lua_State* luaVM);

    CLuaModuleManager* GetLuaModuleManager() const { return m_pLuaModuleManager; }

    void DoPulse();

    void LoadCFunctions();

    std::vector<CLuaMain*>::iterator       begin() { return m_luaContexts.begin(); }
    std::vector<CLuaMain*>::const_iterator begin() const { return m_luaContexts.begin(); }

    std::vector<CLuaMain*>::iterator       end() { return m_luaContexts.end(); }
    std::vector<CLuaMain*>::const_iterator end() const { return m_luaContexts.end(); }

private:
    CBlipManager*        m_pBlipManager;
    CObjectManager*      m_pObjectManager;
    CPlayerManager*      m_pPlayerManager;
    CRadarAreaManager*   m_pRadarAreaManager;
    CRegisteredCommands* m_pRegisteredCommands;
    CVehicleManager*     m_pVehicleManager;
    CMapManager*         m_pMapManager;
    CEvents*             m_pEvents;
    CLuaModuleManager*   m_pLuaModuleManager;

    CFastHashMap<lua_State*, CLuaMain*> m_luaStateToLuaContext;
    std::vector<CLuaMain*>              m_luaContexts;
};
