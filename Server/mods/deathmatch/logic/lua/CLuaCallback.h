/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaCallback.h
 *  PURPOSE:     Lua callback handler
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

//
// For saving a Lua callback function and arguments until needed
//
class CLuaCallback
{
public:
    CLuaCallback(CLuaMain* luaContext, CLuaFunctionRef iLuaFunction, const CLuaArguments& Args)
        : m_pLuaMain(luaContext), m_iLuaFunction(iLuaFunction), m_Arguments(Args)
    {
    }

    void Call()
    {
        if (m_pLuaMain)
            m_Arguments.Call(m_pLuaMain, m_iLuaFunction);
    }

    void OnLuaMainDestroy(CLuaMain* luaContext)
    {
        if (luaContext == m_pLuaMain)
        {
            m_pLuaMain = NULL;
            m_iLuaFunction = CLuaFunctionRef();
            m_Arguments.DeleteArguments();
        }
    }

    bool IsValid() const noexcept { return m_pLuaMain != nullptr; }

protected:
    CLuaMain*       m_pLuaMain;
    CLuaFunctionRef m_iLuaFunction;
    CLuaArguments   m_Arguments;
};

//
// For managing Lua callbacks
//
class CLuaCallbackManager
{
public:
    CLuaCallback* CreateCallback(CLuaMain* luaContext, CLuaFunctionRef iLuaFunction, const CLuaArguments& Args)
    {
        m_CallbackList.push_back(new CLuaCallback(luaContext, iLuaFunction, Args));
        return m_CallbackList.back();
    }

    void DestroyCallback(CLuaCallback* pCallback)
    {
        ListRemove(m_CallbackList, pCallback);
        delete pCallback;
    }

    void OnLuaMainDestroy(CLuaMain* luaContext)
    {
        for (uint i = 0; i < m_CallbackList.size(); i++)
            m_CallbackList[i]->OnLuaMainDestroy(luaContext);
    }

protected:
    std::vector<CLuaCallback*> m_CallbackList;
};
