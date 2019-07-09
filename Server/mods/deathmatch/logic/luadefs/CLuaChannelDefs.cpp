/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaChannelDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaChannelDefs.h"
#include "lua/CLuaChannel.h"
#include "lua/CLuaChannelManager.h"

void CLuaChannelDefs::LoadFunctions()
{
    std::map<const char*, lua_CFunction> functions{
        {"createChannel", CreateChannel},
    };

    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
}

void CLuaChannelDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    // lua_classfunction(luaVM, "create", "setTimer");
    // lua_classvariable(luaVM, "valid", NULL, "isTimer");

    lua_registerclass(luaVM, "Thread");
}

int CLuaChannelDefs::CreateChannel(lua_State* luaVM)
{
    // channel createChannel ( string name )
    SString name;

    CScriptArgReader argStream(luaVM);
    argStream.ReadIfNextIsString(name, "");

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
        return 1;
    }

    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);

    if (!luaMain)
    {
        lua_pushnil(luaVM);
        return 1;
    }

    CLuaChannel* channel = luaMain->GetLuaChannelManager()->CreateChannel(name);

    if (channel)
    {
        lua_pushchannel(luaVM, channel);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}
