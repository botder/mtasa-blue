/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Lua dynamic modules
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "ILuaModuleManager.h"

struct lua_State;

typedef bool (*DefaultModuleFunc)();
typedef void (*RegisterModuleFunc)(lua_State*);
typedef bool (*InitModuleFunc)(ILuaModuleManager*, char*, char*, float*);

struct LuaModuleInfo
{
    // module information
    char    szModuleName[MAX_INFO_LENGTH];
    char    szAuthor[MAX_INFO_LENGTH];
    float   fVersion;
    SString szFileName;

    // module function pointers
    DefaultModuleFunc  ShutdownModule;
    DefaultModuleFunc  DoPulse;
    RegisterModuleFunc RegisterFunctions;

    RegisterModuleFunc ResourceStopping;
    RegisterModuleFunc ResourceStopped;
};

class CLuaModule : public ILuaModuleManager10
{
public:
    CLuaModule(CLuaModuleManager* pLuaModuleManager, CScriptDebugging* pScriptDebugging, const char* szFileName, const char* szShortFileName);
    virtual ~CLuaModule();

    // functions for external modules until DP2.3
    void      Printf(const char* szFormat, ...);
    void      ErrorPrintf(const char* szFormat, ...);
    void      DebugPrintf(lua_State* luaVM, const char* szFormat, ...);
    bool      RegisterFunction(lua_State* luaVM, const char* szFunctionName, lua_CFunction Func);
    bool      GetResourceName(lua_State*   luaVM,
                              std::string& strName);            // This function might not work if module and MTA were compiled with different compiler versions
    CChecksum GetResourceMetaChecksum(lua_State* luaVM);
    CChecksum GetResourceFileChecksum(lua_State* luaVM, const char* szFile);

    // functions for external modules until 1.0
    unsigned long GetVersion();
    const char*   GetVersionString();
    const char*   GetVersionName();
    unsigned long GetNetcodeVersion();
    const char*   GetOperatingSystemName();
    lua_State*    GetResourceFromName(const char* szResourceName);
    // GetResourceName above might not work if module and MTA were compiled with different compiler versions
    bool GetResourceName(lua_State* luaVM, char* szName, size_t length) override;
    bool GetResourceFilePath(lua_State* luaVM, const char* fileName, char* path, size_t length) override;

    // functions for deathmatch
    int  _LoadModule();
    void _UnloadModule();
    void _RegisterFunctions(lua_State* luaVM);
    void _UnregisterFunctions();
    void _DoPulse();
    void _ResourceStopping(lua_State* luaVM);
    void _ResourceStopped(lua_State* luaVM);
    bool _DoesFunctionExist(const char* szFunctionName);

    const SString&       _GetName() const { return m_szShortFileName; }
    const LuaModuleInfo& _GetInfo() const { return m_moduleInfo; }

    // HMODULE      _GetHandle() { return m_hModule; };

private:
    SString              m_szFileName;
    SString              m_szShortFileName;
    LuaModuleInfo        m_moduleInfo;
    void*                m_hModule;
    std::vector<SString> m_Functions;
    bool                 m_bInitialised;

    CScriptDebugging*  m_pScriptDebugging;
    CLuaModuleManager* m_pLuaModuleManager;
};
