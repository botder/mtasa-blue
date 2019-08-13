/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaThread.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include "CLuaThread.h"

CLuaThread::CLuaThread(FromBuffer)
{
    m_ID = CIdArray::PopUniqueId(this, EIdClassType::THREAD);
}

CLuaThread::CLuaThread(FromFile)
{
    m_ID = CIdArray::PopUniqueId(this, EIdClassType::THREAD);
}

CLuaThread::~CLuaThread()
{
    if (m_ID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, EIdClassType::THREAD, m_ID);
        m_ID = INVALID_ARRAY_ID;
    }
}
