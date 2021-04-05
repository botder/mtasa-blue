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
#include <memory>

class CEvents;
class CMapManager;
class CPlayerManager;
class CRegisteredCommands;
class CLuaModuleManager;
class CLuaMain;
struct lua_State;

namespace mtasa
{
    class Resource;
}

class CLuaManager final
{
public:
    CLuaManager(CPlayerManager* playerManager, CRegisteredCommands* registeredCommands, CMapManager* mapManager, CEvents* events);

    ~CLuaManager();

    CLuaMain* CreateLuaContext(mtasa::Resource& resource);
    void      OnLuaStateOpen(CLuaMain* luaContext, lua_State* luaState);
    void      OnLuaStateClose(CLuaMain* luaContext, lua_State* luaState);
    void      DeleteLuaContext(CLuaMain* luaContext);

    CLuaMain*        GetLuaContext(lua_State* luaState);
    mtasa::Resource* GetResourceFromLuaState(lua_State* luaVM);

    CLuaModuleManager*       GetLuaModuleManager() { return m_luaModuleManager.get(); }
    const CLuaModuleManager* GetLuaModuleManager() const { return m_luaModuleManager.get(); }

    void RegisterPluginFunctions(lua_State* luaState) const;

    void DoPulse();
    void LoadCFunctions();

    std::vector<CLuaMain*>::iterator       begin() { return m_luaContexts.begin(); }
    std::vector<CLuaMain*>::const_iterator begin() const { return m_luaContexts.begin(); }

    std::vector<CLuaMain*>::iterator       end() { return m_luaContexts.end(); }
    std::vector<CLuaMain*>::const_iterator end() const { return m_luaContexts.end(); }

private:
    CPlayerManager*      m_playerManager;
    CRegisteredCommands* m_registeredCommands;
    CMapManager*         m_mapManager;
    CEvents*             m_events;

    std::unique_ptr<CLuaModuleManager> m_luaModuleManager;

    CFastHashMap<lua_State*, CLuaMain*> m_luaStateToLuaContext;
    std::vector<CLuaMain*>              m_luaContexts;
};
