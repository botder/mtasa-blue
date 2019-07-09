/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaChannelManager.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaChannelManager.h"
#include "CLuaChannel.h"

void CLuaChannelManager::DoPulse()
{
    // TODO: Add implementation here
}

void CLuaChannelManager::DeleteAll()
{
    m_channelMap.clear();
}

CLuaChannel* CLuaChannelManager::CreateChannel(const SString& name)
{
    // Search for an existing channel by name
    if (!name.empty())
    {
        if (auto iter = m_channelMap.find(name); iter != m_channelMap.end())
        {
            return iter->second.get();
        }
    }

    // Create a new channel
    auto         channelPtr = std::make_unique<CLuaChannel>(name);
    CLuaChannel* channel = channelPtr.get();

    if (name.empty())
    {
        m_unnamedChannels.emplace_back(std::move(channelPtr));
    }
    else
    {
        m_channelMap.emplace(std::make_pair(SString(name), std::move(channelPtr)));
    }

    return channel;
}

bool CLuaChannelManager::DestroyChannel(CLuaChannel* luaChannel)
{
    auto predicate = [luaChannel](const auto& channelPtr) { return channelPtr.get() == luaChannel; };

    if (luaChannel->GetName().empty())
    {
        auto iter = std::find_if(m_unnamedChannels.begin(), m_unnamedChannels.end(), predicate);

        if (iter != m_unnamedChannels.end())
        {
            m_unnamedChannels.erase(iter);
            return true;
        }
    }
    else
    {
        auto iter = std::find_if(m_channelMap.begin(), m_channelMap.end(), predicate);

        if (iter != m_channelMap.end())
        {
            m_channelMap.erase(iter);
            return true;
        }
    }

    return false;
}

CLuaChannel* CLuaChannelManager::GetFromScriptID(SArrayId id) const
{
    auto channel = reinterpret_cast<CLuaChannel*>(CIdArray::FindEntry(id, EIdClass::CHANNEL));

    if (channel)
    {
        auto predicate = [channel](const auto& channelPtr) { return channelPtr.get() == channel; };
        
        if (std::any_of(m_unnamedChannels.begin(), m_unnamedChannels.end(), predicate))
            return channel;

        if (std::any_of(m_channelMap.begin(), m_channelMap.end(), predicate))
            return channel;
    }

    return nullptr;
}
