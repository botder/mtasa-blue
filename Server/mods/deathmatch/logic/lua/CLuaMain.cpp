/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaMain.cpp
 *  PURPOSE:     Lua virtual machine container class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "Resource.h"
#include "ResourceFile.h"
#include "CLuaTimerManager.h"
#include "CLuaFunctionDefs.h"
#include <clocale>

#define HOOK_INSTRUCTION_COUNT 1000000
#define HOOK_MAXIMUM_TIME 5000

using namespace mtasa;

static CLuaManager* m_pLuaManager;
SString             CLuaMain::ms_strExpectedUndumpHash;

extern CGame*      g_pGame;
extern CNetServer* g_pRealNetServer;

#include "luascripts/coroutine_debug.lua.h"
#include "luascripts/exports.lua.h"
#include "luascripts/inspect.lua.h"

CLuaMain::CLuaMain(mtasa::ResourceFile& resourceFile, CLuaManager& luaManager, CPlayerManager* playerManager)
    : CLuaMain(resourceFile.GetResource(), luaManager, playerManager)
{
    m_resourceFile = &resourceFile;
}

CLuaMain::CLuaMain(Resource& resource, CLuaManager& luaManager, CPlayerManager* playerManager)
    : m_resource{resource}, m_manager{luaManager}, m_timerManager{new CLuaTimerManager}, m_playerManager{playerManager}
{
    // TODO: This doesn't belong here, it's a singleton
    m_pLuaManager = &luaManager;

    m_FunctionEnterTimer.SetMaxIncrement(500);
    m_WarningTimer.SetMaxIncrement(1000);

    CPerfStatLuaMemory::GetSingleton()->OnLuaMainCreate(this);
    CPerfStatLuaTiming::GetSingleton()->OnLuaMainCreate(this);
}

CLuaMain::~CLuaMain()
{
    // remove all current remote calls originating from this VM
    g_pGame->GetRemoteCalls()->OnLuaMainDestroy(this);
    g_pGame->GetLuaCallbackManager()->OnLuaMainDestroy(this);
    g_pGame->GetLatentTransferManager()->OnLuaMainDestroy(this);
    g_pGame->GetDebugHookManager()->OnLuaMainDestroy(this);
    g_pGame->GetScriptDebugging()->OnLuaMainDestroy(this);

    // Unload the current script
    CloseMainLuaState();

    // Eventually delete the XML files the LUA script didn't
    for (auto& xmlFile : m_XMLFiles)
    {
        delete xmlFile;
    }

    // Eventually delete the text displays the LUA script didn't
    list<CTextDisplay*>::iterator iterDisplays = m_Displays.begin();
    for (; iterDisplays != m_Displays.end(); ++iterDisplays)
    {
        delete *iterDisplays;
    }

    // Eventually delete the text items the LUA script didn't
    list<CTextItem*>::iterator iterItems = m_TextItems.begin();
    for (; iterItems != m_TextItems.end(); ++iterItems)
    {
        delete *iterItems;
    }

    CPerfStatLuaMemory::GetSingleton()->OnLuaMainDestroy(this);
    CPerfStatLuaTiming::GetSingleton()->OnLuaMainDestroy(this);
}

void CLuaMain::ResetInstructionCount()
{
    m_FunctionEnterTimer.Reset();
}

bool CLuaMain::OpenMainLuaState(LuaContextUseFlags useFlags)
{
    if (m_mainLuaState != nullptr)
        return false;

    lua_State* luaState = lua_open();

    if (luaState == nullptr)
        return false;

    m_useFlags = useFlags;
    m_mainLuaState = luaState;
    m_manager.OnLuaStateOpen(this, luaState);

    // Load LUA libraries
    luaopen_base(luaState);
    luaopen_math(luaState);
    luaopen_string(luaState);
    luaopen_table(luaState);
    luaopen_debug(luaState);
    luaopen_utf8(luaState);
    luaopen_os(luaState);

    // Initialize security restrictions. Very important to prevent lua trojans and viruses!
    ApplyLuaStateSecurity(luaState);

    // Registering C functions
    CLuaCFunctions::RegisterFunctionsWithVM(luaState);

    if (useFlags.useHttp)
        CLuaHTTPDefs::LoadFunctions(luaState);

    // Create class metatables
    lua_initclasses(luaState);
    // lua_newclass(luaVM);

    // Vector and Matrix classes
    CLuaVector4Defs::AddClass(luaState);
    CLuaVector3Defs::AddClass(luaState);
    CLuaVector2Defs::AddClass(luaState);
    CLuaMatrixDefs::AddClass(luaState);

    if (useFlags.useOOP)
        RegisterLuaStateClasses(luaState);

    // Create global variables
    lua_pushelement(luaState, g_pGame->GetMapManager()->GetRootElement());
    lua_setglobal(luaState, "root");

    lua_pushresource(luaState, &m_resource);
    lua_setglobal(luaState, "resource");

    lua_pushelement(luaState, m_resource.GetElement());
    lua_setglobal(luaState, "resourceRoot");

    // Load pre-loaded lua scripts
    LoadScript(EmbeddedLuaCode::exports);
    LoadScript(EmbeddedLuaCode::coroutine_debug);
    LoadScript(EmbeddedLuaCode::inspect);

    // Register C functions from plugins
    if (useFlags.usePlugins)
        m_manager.RegisterPluginFunctions(luaState);

    // Set the instruction count hook
    lua_sethook(luaState, InstructionCountHook, LUA_MASKCOUNT, HOOK_INSTRUCTION_COUNT);

    return true;
}

bool CLuaMain::CloseMainLuaState()
{
    if (m_mainLuaState == nullptr)
        return false;

    // Delete all timers and events
    m_timerManager->RemoveAllTimers();

    // Delete all keybinds
    list<CPlayer*>::const_iterator iter = m_playerManager->IterBegin();
    for (; iter != m_playerManager->IterEnd(); ++iter)
    {
        if ((*iter)->IsJoined())
            (*iter)->GetKeyBinds()->RemoveAllKeys(this);
    }

    // Close our Lua state
    CLuaFunctionRef::RemoveLuaFunctionRefsForVM(m_mainLuaState);
    m_manager.OnLuaStateClose(this, m_mainLuaState);
    lua_close(m_mainLuaState);
    m_mainLuaState = nullptr;
    return true;
}

void CLuaMain::InstructionCountHook(lua_State* luaVM, lua_Debug* pDebug)
{
    if (CLuaMain* luaContext = m_pLuaManager->GetLuaContext(luaVM); luaContext != nullptr)
    {
        // Above max time?
        if (luaContext->m_FunctionEnterTimer.Get() > HOOK_MAXIMUM_TIME)
        {
            // Print it in the console
            const std::string& resourceName = luaContext->GetResourceName();
            CLogger::ErrorPrintf("Infinite/too long execution (%.*s)\n", resourceName.size(), resourceName.c_str());

            // Error out
            std::string message = "Aborting; infinite running script in "s + resourceName;
            luaL_error(luaVM, message.c_str());
        }
    }
}

void CLuaMain::ApplyLuaStateSecurity(lua_State* luaState)
{
    // Disable dangerous Lua Os library functions
    static const luaL_reg osfuncs[] =
    {
        { "execute", CLuaUtilDefs::DisabledFunction },
        { "rename", CLuaUtilDefs::DisabledFunction },
        { "remove", CLuaUtilDefs::DisabledFunction },
        { "exit", CLuaUtilDefs::DisabledFunction },
        { "getenv", CLuaUtilDefs::DisabledFunction },
        { "tmpname", CLuaUtilDefs::DisabledFunction },
        { "setlocale", CLuaUtilDefs::DisabledFunction },
        { NULL, NULL }
    };
    luaL_register(luaState, "os", osfuncs);

    lua_register(luaState, "dofile", CLuaUtilDefs::DisabledFunction);
    lua_register(luaState, "loadfile", CLuaUtilDefs::DisabledFunction);
    lua_register(luaState, "require", CLuaUtilDefs::DisabledFunction);
    lua_register(luaState, "loadlib", CLuaUtilDefs::DisabledFunction);
    lua_register(luaState, "getfenv", CLuaUtilDefs::DisabledFunction);
    lua_register(luaState, "newproxy", CLuaUtilDefs::DisabledFunction);
}

void CLuaMain::RegisterLuaStateClasses(lua_State* luaState)
{
    // NOTE: Element class always comes first due to class inheritance
    CLuaElementDefs::AddClass(luaState);

    CLuaAccountDefs::AddClass(luaState);
    CLuaACLDefs::AddClass(luaState);
    CLuaBanDefs::AddClass(luaState);
    CLuaBlipDefs::AddClass(luaState);
    CLuaColShapeDefs::AddClass(luaState);
    CLuaDatabaseDefs::AddClass(luaState);
    CLuaMarkerDefs::AddClass(luaState);
    CLuaObjectDefs::AddClass(luaState);
    CLuaPedDefs::AddClass(luaState);
    CLuaPickupDefs::AddClass(luaState);
    CLuaPlayerDefs::AddClass(luaState);
    CLuaRadarAreaDefs::AddClass(luaState);
    CLuaResourceDefs::AddClass(luaState);
    CLuaTeamDefs::AddClass(luaState);
    CLuaTextDefs::AddClass(luaState);
    CLuaTimerDefs::AddClass(luaState);
    CLuaVehicleDefs::AddClass(luaState);
    CLuaWaterDefs::AddClass(luaState);

    CLuaShared::AddClasses(luaState);
}

bool CLuaMain::LoadScriptFromBuffer(const char* cpInBuffer, unsigned int uiInSize, const char* szFileName)
{
    SString strNiceFilename = ConformResourcePath(szFileName);

    // Deobfuscate if required
    const char* cpBuffer;
    uint        uiSize;
    if (!g_pRealNetServer->DeobfuscateScript(cpInBuffer, uiInSize, &cpBuffer, &uiSize, strNiceFilename))
    {
        SString strMessage("%s is invalid. Please re-compile at http://luac.mtasa.com/", *strNiceFilename);
        g_pGame->GetScriptDebugging()->LogError(m_mainLuaState, "Loading script failed: %s", *strMessage);
        return false;
    }

    bool bUTF8 = CLuaShared::CheckUTF8BOMAndUpdate(&cpBuffer, &uiSize);

    // If compiled script, make sure correct chunkname is embedded
    CLuaShared::EmbedChunkName(strNiceFilename, &cpBuffer, &uiSize);

    if (m_mainLuaState)
    {
        // Are we not marked as UTF-8 already, and not precompiled?
        std::string strUTFScript;
        if (!bUTF8 && !IsLuaCompiledScript(cpBuffer, uiSize))
        {
            std::string strBuffer = std::string(cpBuffer, uiSize);
#ifdef WIN32
            std::setlocale(LC_CTYPE, "");            // Temporarilly use locales to read the script
            strUTFScript = UTF16ToMbUTF8(ANSIToUTF16(strBuffer));
            std::setlocale(LC_CTYPE, "C");
#else
            strUTFScript = UTF16ToMbUTF8(ANSIToUTF16(strBuffer));
#endif

            if (uiSize != strUTFScript.size())
            {
                uiSize = strUTFScript.size();
                g_pGame->GetScriptDebugging()->LogWarning(m_mainLuaState, "Script '%s' is not encoded in UTF-8.  Loading as ANSI...", strNiceFilename.c_str());
            }
        }
        else
            strUTFScript = std::string(cpBuffer, uiSize);

        // Run the script
        if (CLuaMain::LuaLoadBuffer(m_mainLuaState, bUTF8 ? cpBuffer : strUTFScript.c_str(), uiSize, SString("@%s", *strNiceFilename)))
        {
            // Print the error
            std::string strRes = lua_tostring(m_mainLuaState, -1);
            if (strRes.length())
            {
                CLogger::LogPrintf("SCRIPT ERROR: %s\n", strRes.c_str());
                g_pGame->GetScriptDebugging()->LogError(m_mainLuaState, "Loading script failed: %s", strRes.c_str());
            }
            else
            {
                CLogger::LogPrint("SCRIPT ERROR: Unknown\n");
                g_pGame->GetScriptDebugging()->LogError(m_mainLuaState, "Loading script failed for unknown reason");
            }
        }
        else
        {
            ResetInstructionCount();
            int luaSavedTop = lua_gettop(m_mainLuaState);
            int iret = this->PCall(m_mainLuaState, 0, LUA_MULTRET, 0);
            if (iret == LUA_ERRRUN || iret == LUA_ERRMEM)
            {
                SString strRes = lua_tostring(m_mainLuaState, -1);
                g_pGame->GetScriptDebugging()->LogPCallError(m_mainLuaState, strRes, true);
            }
            // Cleanup any return values
            if (lua_gettop(m_mainLuaState) > luaSavedTop)
                lua_settop(m_mainLuaState, luaSavedTop);
            return true;
        }
    }

    return false;
}

bool CLuaMain::LoadScript(const char* szLUAScript)
{
    if (m_mainLuaState && !IsLuaCompiledScript(szLUAScript, strlen(szLUAScript)))
    {
        // Run the script
        if (!CLuaMain::LuaLoadBuffer(m_mainLuaState, szLUAScript, strlen(szLUAScript), NULL))
        {
            ResetInstructionCount();
            int luaSavedTop = lua_gettop(m_mainLuaState);
            int iret = this->PCall(m_mainLuaState, 0, LUA_MULTRET, 0);
            if (iret == LUA_ERRRUN || iret == LUA_ERRMEM)
            {
                std::string strRes = ConformResourcePath(lua_tostring(m_mainLuaState, -1));
                g_pGame->GetScriptDebugging()->LogPCallError(m_mainLuaState, strRes);
            }
            // Cleanup any return values
            if (lua_gettop(m_mainLuaState) > luaSavedTop)
                lua_settop(m_mainLuaState, luaSavedTop);
        }
        else
        {
            std::string strRes = ConformResourcePath(lua_tostring(m_mainLuaState, -1));
            g_pGame->GetScriptDebugging()->LogError(m_mainLuaState, "Loading in-line script failed: %s", strRes.c_str());
        }
    }
    else
        return false;

    return true;
}

void CLuaMain::DoPulse()
{
    m_timerManager->DoPulse(this);
}

const std::string& CLuaMain::GetResourceName() const
{
    return m_resource.GetName();
}

unsigned long CLuaMain::GetTimerCount() const
{
    return m_timerManager->GetTimerCount();
}

unsigned long CLuaMain::GetElementCount() const
{
    if (CElementGroup* elementGroup = m_resource.GetElementGroup(); elementGroup != nullptr)
        return elementGroup->GetCount();

    return 0;
}

// Keep count of the number of open files in this resource and issue a warning if too high
void CLuaMain::OnOpenFile(const SString& strFilename)
{
    m_OpenFilenameList.push_back(strFilename);
    if (m_OpenFilenameList.size() >= m_uiOpenFileCountWarnThresh)
    {
        m_uiOpenFileCountWarnThresh = m_OpenFilenameList.size() * 2;

        const std::string& resourceName = GetResourceName();
        CLogger::LogPrintf("Notice: There are now %d open files in resource '%.*s'\n", m_OpenFilenameList.size(), resourceName.size(), resourceName.c_str());
    }
}

void CLuaMain::OnCloseFile(const SString& strFilename)
{
    ListRemoveFirst(m_OpenFilenameList, strFilename);
}

CXMLFile* CLuaMain::CreateXML(const char* szFilename, bool bUseIDs, bool bReadOnly)
{
    CXMLFile* pFile = g_pServerInterface->GetXML()->CreateXML(szFilename, bUseIDs, bReadOnly);
    if (pFile)
    {
        m_XMLFiles.push_back(pFile);
        if (m_XMLFiles.size() >= m_uiOpenXMLFileCountWarnThresh)
        {
            m_uiOpenXMLFileCountWarnThresh = m_XMLFiles.size() * 2;

            const std::string& resourceName = GetResourceName();
            CLogger::LogPrintf("Notice: There are now %d open XML files in resource '%.*s'\n", m_XMLFiles.size(), resourceName.size(), resourceName.c_str());
        }
    }
    return pFile;
}

CXMLNode* CLuaMain::ParseString(const char* strXmlContent)
{
    auto xmlStringNode = g_pServerInterface->GetXML()->ParseString(strXmlContent);
    if (!xmlStringNode)
        return nullptr;

    auto node = xmlStringNode->node;
    m_XMLStringNodes.emplace(std::move(xmlStringNode));
    return node;
}

bool CLuaMain::DestroyXML(CXMLFile* pFile)
{
    if (m_XMLFiles.empty())
        return false;
    m_XMLFiles.remove(pFile);
    delete pFile;
    return true;
}

bool CLuaMain::DestroyXML(CXMLNode* pRootNode)
{
    if (m_XMLFiles.empty())
        return false;
    for (CXMLFile* pFile : m_XMLFiles)
    {
        if (pFile)
        {
            if (pFile->GetRootNode() == pRootNode)
            {
                m_XMLFiles.remove(pFile);
                delete pFile;
                break;
            }
        }
    }
    return true;
}

bool CLuaMain::SaveXML(CXMLNode* pRootNode)
{
    for (CXMLFile* pFile : m_XMLFiles)
        if (pFile)
            if (pFile->GetRootNode() == pRootNode)
                return pFile->Write();

    // TODO:
    // list<CResourceFile*>::iterator iter = m_pResource->IterBegin();
    // for (; iter != m_pResource->IterEnd(); ++iter)
    // {
    //     CResourceFile* pResourceFile = *iter;
    //     if (pResourceFile->GetType() == CResourceFile::RESOURCE_FILE_TYPE_CONFIG)
    //     {
    //         CResourceConfigItem* pConfigItem = static_cast<CResourceConfigItem*>(pResourceFile);
    //         if (pConfigItem->GetRoot() == pRootNode)
    //         {
    //             CXMLFile* pFile = pConfigItem->GetFile();
    //             if (pFile)
    //                 return pFile->Write();
    //             return false;
    //         }
    //     }
    // }

    return false;
}

CTextDisplay* CLuaMain::CreateDisplay()
{
    CTextDisplay* pDisplay = new CTextDisplay;
    m_Displays.push_back(pDisplay);
    return pDisplay;
}

void CLuaMain::DestroyDisplay(CTextDisplay* pDisplay)
{
    m_Displays.remove(pDisplay);
    delete pDisplay;
}

CTextItem* CLuaMain::CreateTextItem(const char* szText, float fX, float fY, eTextPriority priority, const SColor color, float fScale, unsigned char format,
                                    unsigned char ucShadowAlpha)
{
    CTextItem* pTextItem = new CTextItem(szText, CVector2D(fX, fY), priority, color, fScale, format, ucShadowAlpha);
    m_TextItems.push_back(pTextItem);
    return pTextItem;
}

void CLuaMain::DestroyTextItem(CTextItem* pTextItem)
{
    m_TextItems.remove(pTextItem);
    delete pTextItem;
}

CTextDisplay* CLuaMain::GetTextDisplayFromScriptID(uint uiScriptID)
{
    CTextDisplay* pTextDisplay = (CTextDisplay*)CIdArray::FindEntry(uiScriptID, EIdClass::TEXT_DISPLAY);
    return pTextDisplay;
}

CTextItem* CLuaMain::GetTextItemFromScriptID(uint uiScriptID)
{
    CTextItem* pTextItem = (CTextItem*)CIdArray::FindEntry(uiScriptID, EIdClass::TEXT_ITEM);
    return pTextItem;
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::GetFunctionTag
//
// Turn iFunctionNumber into something human readable
//
///////////////////////////////////////////////////////////////
const SString& CLuaMain::GetFunctionTag(int iLuaFunction)
{
    // Find existing
    SString* pTag = MapFind(m_FunctionTagMap, iLuaFunction);
#ifndef CHECK_FUNCTION_TAG
    if (!pTag)
#endif
    {
        // Create if required
        SString strText;

        lua_Debug debugInfo;
        lua_getref(m_mainLuaState, iLuaFunction);
        if (lua_getinfo(m_mainLuaState, ">nlS", &debugInfo))
        {
            // Make sure this function isn't defined in a string
            if (debugInfo.source[0] == '@')
            {
                // std::string strFilename2 = ConformResourcePath ( debugInfo.source );
                SString strFilename = debugInfo.source;

                int iPos = strFilename.find_last_of("/\\");
                if (iPos >= 0)
                    strFilename = strFilename.substr(iPos + 1);

                strText = SString("@%s:%d", strFilename.c_str(), debugInfo.currentline != -1 ? debugInfo.currentline : debugInfo.linedefined, iLuaFunction);
            }
            else
            {
                strText = SString("@func_%d %s", iLuaFunction, debugInfo.short_src);
            }
        }
        else
        {
            strText = SString("@func_%d NULL", iLuaFunction);
        }

    #ifdef CHECK_FUNCTION_TAG
        if (pTag)
        {
            // Check tag remains unchanged
            assert(strText == *pTag);
            return *pTag;
        }
    #endif

        MapSet(m_FunctionTagMap, iLuaFunction, strText);
        pTag = MapFind(m_FunctionTagMap, iLuaFunction);
    }
    return *pTag;
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::PCall
//
// lua_pcall call wrapper
//
///////////////////////////////////////////////////////////////
int CLuaMain::PCall(lua_State* L, int nargs, int nresults, int errfunc)
{
    if (m_uiPCallDepth++ == 0)
        m_WarningTimer.Reset();            // Only restart timer if initial call

    g_pGame->GetScriptDebugging()->PushLuaMain(this);
    int iret = lua_pcall(L, nargs, nresults, errfunc);
    g_pGame->GetScriptDebugging()->PopLuaMain(this);

    --m_uiPCallDepth;
    return iret;
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::CheckExecutionTime
//
// Issue warning if execution time is too long
//
///////////////////////////////////////////////////////////////
void CLuaMain::CheckExecutionTime()
{
    // Do time check
    if (m_WarningTimer.Get() < 5000)
        return;
    m_WarningTimer.Reset();

    // No warning if no players
    if (g_pGame->GetPlayerManager()->Count() == 0)
        return;

    // Issue warning about script execution time
    const std::string& resourceName = GetResourceName();
    CLogger::LogPrintf("WARNING: Long execution (%.*s)\n", resourceName.size(), resourceName.c_str());
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::LuaLoadBuffer
//
// luaL_loadbuffer call wrapper
//
///////////////////////////////////////////////////////////////
int CLuaMain::LuaLoadBuffer(lua_State* L, const char* buff, size_t sz, const char* name)
{
    if (IsLuaCompiledScript(buff, sz))
    {
        ms_strExpectedUndumpHash = GenerateSha256HexString(buff, sz);
    }

    int iResult = luaL_loadbuffer(L, buff, sz, name);

    ms_strExpectedUndumpHash = "";
    return iResult;
}

///////////////////////////////////////////////////////////////
//
// CLuaMain::OnUndump
//
// Callback from Lua when loading compiled bytes
//
///////////////////////////////////////////////////////////////
int CLuaMain::OnUndump(const char* p, size_t n)
{
    SString strGotHash = GenerateSha256HexString(p, n);
    SString strExpectedHash = ms_strExpectedUndumpHash;
    ms_strExpectedUndumpHash = "";
    if (strExpectedHash != strGotHash)
    {
        // Not right
        return 0;
    }
    return 1;
}
