/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaMain.h
 *  PURPOSE:     Lua virtual machine container class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "lua/CLuaVector2.h"
#include "lua/CLuaVector3.h"
#include "lua/CLuaVector4.h"
#include "lua/CLuaMatrix.h"
#include "CLuaModuleManager.h"
#include "../CTextDisplay.h"
#include "CLuaFunctionDefs.h"
#include <string>
#include <memory>

#define MAX_SCRIPTNAME_LENGTH 64

class CBlipManager;
class CObjectManager;
class CPlayerManager;
class CRadarAreaManager;
class CVehicleManager;
class CMapManager;
class CResourceFile;
class CLuaManager;
class CLuaTimerManager;

namespace mtasa
{
    class Resource;
    class ResourceFile;
}

struct CRefInfo
{
    unsigned long int ulUseCount;
    int               iFunction;
};

struct LuaContextUseFlags
{
    bool useOOP = false;
    bool useHttp = false;
    bool usePlugins = true;
};

class CLuaMain final
{
public:
    ZERO_ON_NEW

    CLuaMain(mtasa::ResourceFile& resourceFile, CLuaManager& luaManager, CPlayerManager* playerManager);
    CLuaMain(mtasa::Resource& resource, CLuaManager& luaManager, CPlayerManager* playerManager);

    ~CLuaMain();

    bool LoadScriptFromBuffer(const char* cpBuffer, unsigned int uiSize, const char* szFileName);
    bool LoadScript(const char* szLUAScript);

    void DoPulse();

    CXMLFile*     CreateXML(const char* szFilename, bool bUseIDs = true, bool bReadOnly = false);
    CXMLNode*     ParseString(const char* strXmlContent);
    bool          DestroyXML(CXMLFile* pFile);
    bool          DestroyXML(CXMLNode* pRootNode);
    bool          SaveXML(CXMLNode* pRootNode);
    unsigned long GetXMLFileCount() const { return m_XMLFiles.size(); }
    unsigned long GetOpenFileCount() const { return m_OpenFilenameList.size(); }
    unsigned long GetTimerCount() const;
    unsigned long GetElementCount() const;
    unsigned long GetTextDisplayCount() const { return m_Displays.size(); }
    unsigned long GetTextItemCount() const { return m_TextItems.size(); }
    void          OnOpenFile(const SString& strFilename);
    void          OnCloseFile(const SString& strFilename);

    CTextDisplay* CreateDisplay();
    void          DestroyDisplay(CTextDisplay* pDisplay);
    CTextItem*    CreateTextItem(const char* szText, float fX, float fY, eTextPriority priority = PRIORITY_LOW, const SColor color = -1, float fScale = 1.0f,
                                 unsigned char format = 0, unsigned char ucShadowAlpha = 0);
    void          DestroyTextItem(CTextItem* pTextItem);

    CTextDisplay* GetTextDisplayFromScriptID(uint uiScriptID);
    CTextItem*    GetTextItemFromScriptID(uint uiScriptID);

    void ResetInstructionCount();

    mtasa::Resource&       GetResource() { return m_resource; }
    const mtasa::Resource& GetResource() const { return m_resource; }

    const SString& GetFunctionTag(int iFunctionNumber);
    int            PCall(lua_State* L, int nargs, int nresults, int errfunc);
    void           CheckExecutionTime();
    static int     LuaLoadBuffer(lua_State* L, const char* buff, size_t sz, const char* name);
    static int     OnUndump(const char* p, size_t n);

public:
    bool OpenMainLuaState(LuaContextUseFlags useFlags = {});
    bool CloseMainLuaState();

    const std::string& GetResourceName() const;

    mtasa::ResourceFile*       GetResourceFile() { return m_resourceFile; }
    const mtasa::ResourceFile* GetResourceFile() const { return m_resourceFile; }

    lua_State* GetMainLuaState() const { return m_mainLuaState; }

    bool IsUsingOOP() const { return m_useFlags.useOOP; }

    // TODO: Return a reference
    CLuaTimerManager* GetTimerManager() const { return m_timerManager.get(); }

private:
    static void InstructionCountHook(lua_State* luaVM, lua_Debug* pDebug);
    static void ApplyLuaStateSecurity(lua_State* luaState);
    static void RegisterLuaStateClasses(lua_State* luaState);

    mtasa::Resource&     m_resource;
    mtasa::ResourceFile* m_resourceFile = nullptr;
    CLuaManager&         m_manager;
    LuaContextUseFlags   m_useFlags;
    lua_State*           m_mainLuaState = nullptr;

    std::unique_ptr<CLuaTimerManager> m_timerManager;
    CPlayerManager*                   m_playerManager;

private:
    list<CXMLFile*>                                 m_XMLFiles;
    std::unordered_set<std::unique_ptr<SXMLString>> m_XMLStringNodes;
    list<CTextDisplay*>                             m_Displays;
    list<CTextItem*>                                m_TextItems;

    CElapsedTime         m_FunctionEnterTimer;
    CElapsedTimeApprox   m_WarningTimer;
    uint                 m_uiPCallDepth = 0;
    std::vector<SString> m_OpenFilenameList;
    uint                 m_uiOpenFileCountWarnThresh = 10;
    uint                 m_uiOpenXMLFileCountWarnThresh = 20;
    static SString       ms_strExpectedUndumpHash;

public:
    CFastHashMap<const void*, CRefInfo> m_CallbackTable;
    std::map<int, SString>              m_FunctionTagMap;
};
