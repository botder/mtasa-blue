/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CLuaEventPacket.cpp
 *  PURPOSE:     Lua event packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CLuaEventPacket::CLuaEventPacket(void)
{
    m_ElementID = INVALID_ELEMENT_ID;
    m_pArguments = &m_ArgumentsStore;
}

CLuaEventPacket::CLuaEventPacket(const char* szName, ElementID ID, CLuaArguments* pArguments)
{
    m_strName.AssignLeft(szName, MAX_EVENT_NAME_LENGTH);
    m_ElementID = ID;
    m_pArguments = pArguments;            // Use a pointer to save copying the arguments

    if (g_pGame->GetDevelopmentMode())
    {
        CLuaMain* pLuaMain = g_pGame->GetScriptDebugging()->GetTopLuaMain();

        if (pLuaMain)
        {
            const SLuaDebugInfo& luaDebugInfo = g_pGame->GetScriptDebugging()->GetLuaDebugInfo(pLuaMain->GetVirtualMachine());

            if (luaDebugInfo.infoType == DEBUG_INFO_FILE_AND_LINE && !luaDebugInfo.strFile.empty())
            {
                m_strDebugInfo = SString("(in resource %s in file %s on line %d)", pLuaMain->GetScriptName(), luaDebugInfo.strFile.c_str(), 
                                         luaDebugInfo.iLine);
            }
        }
    }
}

bool CLuaEventPacket::Read(NetBitStreamInterface& BitStream)
{
    unsigned short usNameLength;

    if (!BitStream.ReadCompressed(usNameLength))
        return false;

    if (usNameLength > (MAX_EVENT_NAME_LENGTH - 1))
        return false;

    if (!BitStream.ReadStringCharacters(m_strName, usNameLength) || !BitStream.Read(m_ElementID))
        return false;

    // Faster than using a constructor
    m_ArgumentsStore.DeleteArguments();

    if (!m_ArgumentsStore.ReadFromBitStream(BitStream))
        return false;

    m_pArguments = &m_ArgumentsStore;

    // Read optional debug information
    if (BitStream.Version() >= 0x06D)
    {
        if (BitStream.ReadBit())
        {
            if (!BitStream.ReadString(m_strDebugInfo))
                return false;
        }
    }

    return true;
}

bool CLuaEventPacket::Write(NetBitStreamInterface& BitStream) const
{
    unsigned short usNameLength = static_cast<unsigned short>(m_strName.length());
    BitStream.WriteCompressed(usNameLength);
    BitStream.WriteStringCharacters(m_strName, usNameLength);
    BitStream.Write(m_ElementID);
    m_pArguments->WriteToBitStream(BitStream);

    if (BitStream.Version() >= 0x06D)
    {
        if (!m_strDebugInfo.empty())
        {
            BitStream.WriteBit(true);
            BitStream.WriteString(m_strDebugInfo);
        }
        else
            BitStream.WriteBit(false);
    }

    return true;
}
