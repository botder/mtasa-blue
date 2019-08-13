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
class CLuaChannelManager;

#include "CLuaThread.h"
#include <vector>
#include <memory>

class CLuaThreadManager
{
public:
    CLuaThreadManager(CLuaMain* luaMain) : m_luaMain(luaMain) {}

    void DoPulse();
    void DeleteAll();

    void SetChannelManager(CLuaChannelManager* channelManager) { m_channelManager = channelManager; }

    CLuaThread* CreateThreadFromSource(const SString& sourceCode);
    CLuaThread* CreateThreadFromFile(const SString& filePath);
    bool        DestroyThread(CLuaThread* thread);

    CLuaThread* GetFromScriptID(SArrayId id) const;

private:
    CLuaMain* m_luaMain;
    CLuaChannelManager* m_channelManager = nullptr;
    std::vector<std::unique_ptr<CLuaThread>> m_threads;
};
