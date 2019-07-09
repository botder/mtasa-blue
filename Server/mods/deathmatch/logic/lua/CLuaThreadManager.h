/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaThreadManager.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

class CLuaMain;
class CLuaThread;
class CLuaChannelManager;

#include <vector>

class CLuaThreadManager
{
public:
    void DoPulse();
    void DeleteAll();

    void SetChannelManager(CLuaChannelManager* channelManager) { m_channelManager = channelManager; }

    CLuaThread* GetFromScriptID(SArrayId id) const;

private:
    CLuaMain* m_luaMain;
    CLuaChannelManager* m_channelManager;
    std::vector<CLuaThread> m_threads;
};
