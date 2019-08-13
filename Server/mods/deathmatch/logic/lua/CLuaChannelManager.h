/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaChannelManager.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

class CLuaMain;
class CLuaThreadManager;

#include "CLuaChannel.h"
#include <vector>
#include <unordered_map>
#include <memory>

class CLuaChannelManager
{
public:
    CLuaChannelManager(CLuaMain* luaMain) : m_luaMain(luaMain) {}

    void DoPulse();
    void DeleteAll();

    void SetThreadManager(CLuaThreadManager* threadManager) { m_threadManager = threadManager; }

    CLuaChannel* CreateChannel(const SString& name);
    bool         DestroyChannel(CLuaChannel* channel);

    CLuaChannel* GetFromScriptID(SArrayId id) const;

private:
    CLuaMain*                                                 m_luaMain;
    CLuaThreadManager*                                        m_threadManager = nullptr;
    std::vector<std::unique_ptr<CLuaChannel>>                 m_unnamedChannels;
    std::unordered_map<SString, std::unique_ptr<CLuaChannel>> m_channelMap;
};
