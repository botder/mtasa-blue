/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaChannel.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CIdArray.h"
#include "LuaCommon.h"

class CLuaChannel
{
public:
    CLuaChannel(const SString& name);
    ~CLuaChannel();

    SArrayId GetScriptID() const { return m_ID; }

    const SLuaDebugInfo& GetLuaDebugInfo() const { return m_luaDebugInfo; }
    void                 SetLuaDebugInfo(const SLuaDebugInfo& luaDebugInfo) { m_luaDebugInfo = luaDebugInfo; }

    const SString& GetName() const { return m_Name; }

private:
    SArrayId      m_ID = INVALID_ARRAY_ID;
    SLuaDebugInfo m_luaDebugInfo;
    SString       m_Name;
};
