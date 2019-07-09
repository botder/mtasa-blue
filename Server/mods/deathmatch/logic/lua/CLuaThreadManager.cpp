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
    // TODO: Add implementation here
}

CLuaThread* CLuaThreadManager::GetFromScriptID(SArrayId id) const
{
    auto thread = reinterpret_cast<CLuaThread*>(CIdArray::FindEntry(id, EIdClass::THREAD));

    if (thread)
    {
        auto predicate = [thread](const auto& threadPtr) { return threadPtr.get() == thread; };

        if (std::any_of(m_threads.begin(), m_threads.end(), predicate))
            return thread;
    }

    return nullptr;
}
