/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaThreadDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaThreadDefs.h"
#include "lua/CLuaThread.h"
#include "lua/CLuaThreadManager.h"

void CLuaThreadDefs::LoadFunctions()
{
    std::map<const char*, lua_CFunction> functions{
        {"createThread", CreateThread},
    };

    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
}

void CLuaThreadDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    // lua_classfunction(luaVM, "create", "setTimer");
    // lua_classvariable(luaVM, "valid", NULL, "isTimer");

    lua_registerclass(luaVM, "Thread");
}

int CLuaThreadDefs::CreateThread(lua_State* luaVM)
{
    // thread createThread ( string filePath/sourceCode [, bool fromFile = true ] )
    SString source;
    bool    fromFile;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(source);
    argStream.ReadBool(fromFile, true);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
        return 1;
    }

    if (source.empty())
    {
        m_pScriptDebugging->LogCustom(luaVM, "expected non-empty string at argument 1");
        lua_pushnil(luaVM);
        return 1;
    }

    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);

    if (!luaMain)
    {
        lua_pushnil(luaVM);
        return 1;
    }

    CLuaThread* thread = nullptr;

    if (fromFile)
    {
        CResource* currentResource = luaMain->GetResource();
        CResource* targetResource = currentResource;
        SString    path, metaPath;

        if (CResourceManager::ParseResourcePathInput(source, targetResource, &path, &metaPath))
        {
            if (FileExists(path))
            {
                CheckCanModifyOtherResource(argStream, currentResource, targetResource);

                if (!argStream.HasErrors())
                {
                    thread = luaMain->GetLuaThreadManager()->CreateThreadFromFile(path);
                }
            }
            else
            {
                argStream.SetCustomError(source, "File not found");
            }
        }
        else
        {
            argStream.SetCustomError(source, "Bad file path");
        }
    }
    else
    {
        thread = luaMain->GetLuaThreadManager()->CreateThreadFromSource(source);
    }

    if (!argStream.HasErrors() && thread)
    {
        lua_pushthread(luaVM, thread);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}
