/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaHTTPDefs.cpp
 *  PURPOSE:     Lua HTTP webserver definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ResourceHttpFile.h"

using namespace mtasa;

static ResourceHttpFile* GetResourceHttpFile(lua_State* luaState, CLuaManager& luaManager)
{
    if (CLuaMain* luaContext = luaManager.GetLuaContext(luaState); luaContext != nullptr)
    {
        if (ResourceFile* file = luaContext->GetResourceFile(); file != nullptr)
        {
            if (file->GetType() == ResourceFileType::SERVER_HTTP)
                return static_cast<ResourceHttpFile*>(file);
        }
    }

    return nullptr;
}

void CLuaHTTPDefs::LoadFunctions(lua_State* luaState)
{
    // Register these funcs for that VM
    lua_register(luaState, "httpWrite", CLuaHTTPDefs::httpWrite);
    lua_register(luaState, "httpSetResponseHeader", CLuaHTTPDefs::httpSetResponseHeader);
    lua_register(luaState, "httpSetResponseCookie", CLuaHTTPDefs::httpSetResponseCookie);
    lua_register(luaState, "httpSetResponseCode", CLuaHTTPDefs::httpSetResponseCode);
    lua_register(luaState, "httpClear", CLuaHTTPDefs::httpClear);
    lua_register(luaState, "httpRequestLogin", CLuaHTTPDefs::httpRequestLogin);
}

int CLuaHTTPDefs::httpWrite(lua_State* luaState)
{
    ResourceHttpFile* httpFile = GetResourceHttpFile(luaState, *m_pLuaManager);

    if (httpFile == nullptr)
    {
        m_pScriptDebugging->LogError(luaState, "%s: Can only be used in HTML scripts", lua_tostring(luaState, lua_upvalueindex(1)));
        lua_pushboolean(luaState, false);
        return 1;
    }

    // bool httpWrite ( string data [, int length] )
    std::string_view data;
    std::size_t      length = -1;

    CScriptArgReader argStream(luaState);
    argStream.ReadStringView(data);
    argStream.ReadNumber(length, -1);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaState, argStream.GetFullErrorMessage());
        lua_pushboolean(luaState, false);
        return 1;
    }

    httpFile->AppendToPageBuffer(data.substr(0, length));
    lua_pushboolean(luaState, true);
    return 1;
}

int CLuaHTTPDefs::httpSetResponseHeader(lua_State* luaState)
{
    ResourceHttpFile* httpFile = GetResourceHttpFile(luaState, *m_pLuaManager);

    if (httpFile == nullptr)
    {
        m_pScriptDebugging->LogError(luaState, "%s: Can only be used in HTML scripts", lua_tostring(luaState, lua_upvalueindex(1)));
        lua_pushboolean(luaState, false);
        return 1;
    }

    // bool httpSetResponseHeader ( string headerName, string headerValue )
    std::string_view name;
    std::string_view value;

    CScriptArgReader argStream(luaState);
    argStream.ReadStringView(name);
    argStream.ReadStringView(value);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaState, argStream.GetFullErrorMessage());
        lua_pushboolean(luaState, false);
        return 1;
    }

    httpFile->SetResponseHeader(name, value);
    lua_pushboolean(luaState, true);
    return 1;
}

int CLuaHTTPDefs::httpSetResponseCookie(lua_State* luaState)
{
    ResourceHttpFile* httpFile = GetResourceHttpFile(luaState, *m_pLuaManager);

    if (httpFile == nullptr)
    {
        m_pScriptDebugging->LogError(luaState, "%s: Can only be used in HTML scripts", lua_tostring(luaState, lua_upvalueindex(1)));
        lua_pushboolean(luaState, false);
        return 1;
    }

    // bool httpSetResponseCookie ( string cookieName, string cookieValue )
    std::string_view name;
    std::string_view value;

    CScriptArgReader argStream(luaState);
    argStream.ReadStringView(name);
    argStream.ReadStringView(value);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaState, argStream.GetFullErrorMessage());
        lua_pushboolean(luaState, false);
        return 1;
    }

    httpFile->SetResponseCookie(name, value);
    lua_pushboolean(luaState, true);
    return 1;
}

int CLuaHTTPDefs::httpSetResponseCode(lua_State* luaState)
{
    ResourceHttpFile* httpFile = GetResourceHttpFile(luaState, *m_pLuaManager);

    if (httpFile == nullptr)
    {
        m_pScriptDebugging->LogError(luaState, "%s: Can only be used in HTML scripts", lua_tostring(luaState, lua_upvalueindex(1)));
        lua_pushboolean(luaState, false);
        return 1;
    }

    // bool httpSetResponseHeader ( string headerName, string headerValue )
    std::uint16_t responseCode;

    CScriptArgReader argStream(luaState);
    argStream.ReadNumber(responseCode);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaState, argStream.GetFullErrorMessage());
        lua_pushboolean(luaState, false);
        return 1;
    }

    httpFile->SetResponseCode(responseCode);
    lua_pushboolean(luaState, true);
    return 1;
}

int CLuaHTTPDefs::httpClear(lua_State* luaState)
{
    ResourceHttpFile* httpFile = GetResourceHttpFile(luaState, *m_pLuaManager);

    if (httpFile == nullptr)
    {
        m_pScriptDebugging->LogError(luaState, "%s: Can only be used in HTML scripts", lua_tostring(luaState, lua_upvalueindex(1)));
        lua_pushboolean(luaState, false);
        return 1;
    }

    // bool httpClear ( )
    httpFile->ClearPageBuffer();
    lua_pushboolean(luaState, true);
    return 1;
}

int CLuaHTTPDefs::httpRequestLogin(lua_State* luaState)
{
    ResourceHttpFile* httpFile = GetResourceHttpFile(luaState, *m_pLuaManager);

    if (httpFile == nullptr)
    {
        m_pScriptDebugging->LogError(luaState, "%s: Can only be used in HTML scripts", lua_tostring(luaState, lua_upvalueindex(1)));
        lua_pushboolean(luaState, false);
        return 1;
    }

    // bool httpRequestLogin ( )
    char realm[255];
    snprintf(realm, 255, "Basic realm=\"%s\"", m_pMainConfig->GetServerName().c_str());
    httpFile->SetResponseHeader("WWW-Authenticate", realm);
    httpFile->SetResponseCode(401);
    lua_pushboolean(luaState, true);
    return 1;
}
