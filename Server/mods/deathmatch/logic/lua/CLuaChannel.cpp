/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaChannel.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaChannel.h"

CLuaChannel::CLuaChannel(const SString& name) : m_Name(name)
{
    m_ID = CIdArray::PopUniqueId(this, EIdClassType::CHANNEL);
}

CLuaChannel::~CLuaChannel()
{
    if (m_ID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, EIdClassType::CHANNEL, m_ID);
        m_ID = INVALID_ARRAY_ID;
    }
}
