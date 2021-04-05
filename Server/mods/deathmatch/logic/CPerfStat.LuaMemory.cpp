/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPerfStat.LuaMemory.cpp
 *  PURPOSE:     Performance stats manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

namespace
{
    //
    // CLuaMainMemory
    //
    class CLuaMainMemory
    {
    public:
        CLuaMainMemory() { memset(this, 0, sizeof(*this)); }

        int Delta;
        int Current;
        int Max;
        int OpenXMLFiles;
        int OpenFiles;
        int Refs;
        int TimerCount;
        int ElementCount;
        int TextDisplayCount;
        int TextItemCount;
    };

    typedef std::map<CLuaMain*, CLuaMainMemory> CLuaMainMemoryMap;
    class CAllLuaMemory
    {
    public:
        CLuaMainMemoryMap LuaMainMemoryMap;
    };
}            // namespace

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaMemoryImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatLuaMemoryImpl : public CPerfStatLuaMemory
{
public:
    ZERO_ON_NEW
    CPerfStatLuaMemoryImpl();
    virtual ~CPerfStatLuaMemoryImpl();

    // CPerfStatModule
    virtual const SString& GetCategoryName();
    virtual void           DoPulse();
    virtual void           GetStats(CPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter);

    // CPerfStatLuaMemory
    virtual void OnLuaMainCreate(CLuaMain* luaContext);
    virtual void OnLuaMainDestroy(CLuaMain* luaContext);

    // CPerfStatLuaMemoryImpl
    void GetLuaMemoryStats(CPerfStatResult* pResult, const std::map<SString, int>& strOptionMap, const SString& strFilter);
    void UpdateLuaMemory(CLuaMain* luaContext, int iMemUsed);

    SString                  m_strCategoryName;
    CAllLuaMemory            AllLuaMemory;
    std::map<CLuaMain*, int> m_LuaMainMap;
};

///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static std::unique_ptr<CPerfStatLuaMemoryImpl> g_pPerfStatLuaMemoryImp;

CPerfStatLuaMemory* CPerfStatLuaMemory::GetSingleton()
{
    if (!g_pPerfStatLuaMemoryImp)
        g_pPerfStatLuaMemoryImp.reset(new CPerfStatLuaMemoryImpl());
    return g_pPerfStatLuaMemoryImp.get();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaMemoryImpl::CPerfStatLuaMemoryImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatLuaMemoryImpl::CPerfStatLuaMemoryImpl()
{
    m_strCategoryName = "Lua memory";
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaMemoryImpl::CPerfStatLuaMemoryImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatLuaMemoryImpl::~CPerfStatLuaMemoryImpl()
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaMemoryImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatLuaMemoryImpl::GetCategoryName()
{
    return m_strCategoryName;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaMemoryImpl::OnLuaMainCreate
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaMemoryImpl::OnLuaMainCreate(CLuaMain* luaContext)
{
    MapSet(m_LuaMainMap, luaContext, 1);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaMemoryImpl::OnLuaMainDestroy
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaMemoryImpl::OnLuaMainDestroy(CLuaMain* luaContext)
{
    MapRemove(m_LuaMainMap, luaContext);
    MapRemove(AllLuaMemory.LuaMainMemoryMap, luaContext);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaMemoryImpl::LuaMemory
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaMemoryImpl::UpdateLuaMemory(CLuaMain* luaContext, int iMemUsed)
{
    CLuaMainMemory* pLuaMainMemory = MapFind(AllLuaMemory.LuaMainMemoryMap, luaContext);
    if (!pLuaMainMemory)
    {
        MapSet(AllLuaMemory.LuaMainMemoryMap, luaContext, CLuaMainMemory());
        pLuaMainMemory = MapFind(AllLuaMemory.LuaMainMemoryMap, luaContext);
    }

    pLuaMainMemory->Delta += iMemUsed - pLuaMainMemory->Current;
    pLuaMainMemory->Current = iMemUsed;
    pLuaMainMemory->Max = std::max(pLuaMainMemory->Max, pLuaMainMemory->Current);

    pLuaMainMemory->OpenXMLFiles = luaContext->GetXMLFileCount();
    pLuaMainMemory->OpenFiles = luaContext->GetOpenFileCount();
    pLuaMainMemory->Refs = luaContext->m_CallbackTable.size();
    pLuaMainMemory->TimerCount = luaContext->GetTimerCount();
    pLuaMainMemory->ElementCount = luaContext->GetElementCount();
    pLuaMainMemory->TextDisplayCount = luaContext->GetTextDisplayCount();
    pLuaMainMemory->TextItemCount = luaContext->GetTextItemCount();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaMemoryImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaMemoryImpl::DoPulse()
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaMemoryImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaMemoryImpl::GetStats(CPerfStatResult* pResult, const std::map<SString, int>& optionMap, const SString& strFilter)
{
    pResult->Clear();
    GetLuaMemoryStats(pResult, optionMap, strFilter);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaMemoryImpl::GetLuaMemoryStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaMemoryImpl::GetLuaMemoryStats(CPerfStatResult* pResult, const std::map<SString, int>& strOptionMap, const SString& strFilter)
{
    //
    // Set option flags
    //
    bool bHelp = MapContains(strOptionMap, "h");
    bool bAccurate = MapContains(strOptionMap, "a");

    //
    // Process help
    //
    if (bHelp)
    {
        pResult->AddColumn("Lua memory help");
        pResult->AddRow()[0] = "Option h - This help";
        pResult->AddRow()[0] = "Option a - More accurate memory usage - Warning: Can slow server a little";
        return;
    }

    // Fetch mem stats from Lua
    {
        for (const auto& [luaContext, unused] : m_LuaMainMap)
        {
            if (lua_State* luaState = luaContext->GetMainLuaState())
            {
                if (bAccurate)
                    lua_gc(luaState, LUA_GCCOLLECT, 0);

                int iMemUsed = lua_getgccount(luaState);
                UpdateLuaMemory(luaContext, iMemUsed);
            }
        }
    }

    pResult->AddColumn("name");
    pResult->AddColumn("change");
    pResult->AddColumn("current");
    pResult->AddColumn("max");
    pResult->AddColumn("XMLFiles");
    pResult->AddColumn("OpenFiles");
    pResult->AddColumn("refs");
    pResult->AddColumn("Timers");
    pResult->AddColumn("Elements");
    pResult->AddColumn("TextDisplays");
    pResult->AddColumn("TextItems");
    pResult->AddColumn("DB Queries");
    pResult->AddColumn("DB Connections");

    // Calc totals
    if (strFilter == "")
    {
        int calcedCurrent = 0;
        int calcedDelta = 0;
        int calcedMax = 0;
        for (CLuaMainMemoryMap::iterator iter = AllLuaMemory.LuaMainMemoryMap.begin(); iter != AllLuaMemory.LuaMainMemoryMap.end(); ++iter)
        {
            CLuaMainMemory& LuaMainMemory = iter->second;
            calcedCurrent += LuaMainMemory.Current;
            calcedDelta += LuaMainMemory.Delta;
            calcedMax += LuaMainMemory.Max;
        }

        // Add row
        SString* row = pResult->AddRow();

        int c = 0;
        row[c++] = "Lua VM totals";

        if (labs(calcedDelta) >= 1)
        {
            row[c] = SString("%d KB", calcedDelta);
            calcedDelta = 0;
        }
        c++;

        row[c++] = SString("%d KB", calcedCurrent);
        row[c++] = SString("%d KB", calcedMax);

        // Some extra 'all VM' things
        c += 6;
        row[c++] = !g_pStats->iDbJobDataCount ? "-" : SString("%d", g_pStats->iDbJobDataCount);
        row[c++] = g_pStats->iDbConnectionCount - 2 == 0 ? "-" : SString("%d", g_pStats->iDbConnectionCount - 2);
    }

    // For each VM
    for (CLuaMainMemoryMap::iterator iter = AllLuaMemory.LuaMainMemoryMap.begin(); iter != AllLuaMemory.LuaMainMemoryMap.end(); ++iter)
    {
        CLuaMainMemory&    LuaMainMemory = iter->second;
        const std::string& resourceName = iter->first->GetResourceName();

        // Apply filter
        if (strFilter != "" && resourceName.find(strFilter) == std::string::npos)
            continue;

        // Add row
        SString* row = pResult->AddRow();

        int c = 0;
        row[c++] = resourceName;

        if (labs(LuaMainMemory.Delta) >= 1)
        {
            row[c] = SString("%d KB", LuaMainMemory.Delta);
            LuaMainMemory.Delta = 0;
        }
        c++;

        row[c++] = SString("%d KB", LuaMainMemory.Current);
        row[c++] = SString("%d KB", LuaMainMemory.Max);
        row[c++] = !LuaMainMemory.OpenXMLFiles ? "-" : SString("%d", LuaMainMemory.OpenXMLFiles);
        row[c++] = !LuaMainMemory.OpenFiles ? "-" : SString("%d", LuaMainMemory.OpenFiles);
        row[c++] = !LuaMainMemory.Refs ? "-" : SString("%d", LuaMainMemory.Refs);
        row[c++] = !LuaMainMemory.TimerCount ? "-" : SString("%d", LuaMainMemory.TimerCount);
        row[c++] = !LuaMainMemory.ElementCount ? "-" : SString("%d", LuaMainMemory.ElementCount);
        row[c++] = !LuaMainMemory.TextDisplayCount ? "-" : SString("%d", LuaMainMemory.TextDisplayCount);
        row[c++] = !LuaMainMemory.TextItemCount ? "-" : SString("%d", LuaMainMemory.TextItemCount);
    }
}
