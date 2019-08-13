/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaThreadManager.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaThreadManager.h"

void CLuaThreadManager::DoPulse()
{
    // TODO: Add implementation here
}

void CLuaThreadManager::DeleteAll()
{
    m_threads.clear();
}

CLuaThread* CLuaThreadManager::CreateThreadFromSource(const SString& sourceCode)
{
    return nullptr;
}

CLuaThread* CLuaThreadManager::CreateThreadFromFile(const SString& filePath)
{
    return nullptr;
}

bool CLuaThreadManager::DestroyThread(CLuaThread* thread)
{
    const auto predicate = [thread](const std::unique_ptr<CLuaThread>& threadPtr) { return threadPtr.get() == thread; };

    if (auto iter = std::find_if(m_threads.begin(), m_threads.end(), predicate); iter != m_threads.end())
    {
        m_threads.erase(iter);
        return true;
    }

    return false;
}

CLuaThread* CLuaThreadManager::GetFromScriptID(SArrayId id) const
{
    auto thread = reinterpret_cast<CLuaThread*>(CIdArray::FindEntry(id, EIdClass::THREAD));

    if (thread)
    {
        auto predicate = [thread](const std::unique_ptr<CLuaThread>& threadPtr) { return threadPtr.get() == thread; };

        if (std::any_of(m_threads.begin(), m_threads.end(), predicate))
            return thread;
    }

    return nullptr;
}
