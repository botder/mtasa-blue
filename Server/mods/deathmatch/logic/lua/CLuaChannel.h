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

class CLuaChannel
{
public:
    CLuaChannel(const SString& name);
    ~CLuaChannel();

    SArrayId GetScriptID() const { return m_ID; }

    const SString& GetName() const { return m_Name; }

private:
    SArrayId m_ID = INVALID_ARRAY_ID;
    SString  m_Name;
};
