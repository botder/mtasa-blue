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
    if (luaChannel->GetName().empty())
    {
        const auto predicate = [luaChannel](const std::unique_ptr<CLuaChannel>& channelPtr) { return channelPtr.get() == luaChannel; };
        auto iter = std::find_if(m_unnamedChannels.begin(), m_unnamedChannels.end(), predicate);

        if (iter != m_unnamedChannels.end())
        {
            m_unnamedChannels.erase(iter);
            return true;
        }
    }
    else
    {
        auto iter = std::find_if(m_channelMap.begin(), m_channelMap.end(), [luaChannel](const auto& pair) { return pair.second.get() == luaChannel; });

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
        auto unnamedPredicate = [channel](const std::unique_ptr<CLuaChannel>& channelPtr) { return channelPtr.get() == channel; };
        
        if (std::any_of(m_unnamedChannels.begin(), m_unnamedChannels.end(), unnamedPredicate))
            return channel;

        auto mapPredicate = [channel](const auto& pair) { return pair.second.get() == channel; };

        if (std::any_of(m_channelMap.begin(), m_channelMap.end(), mapPredicate))
            return channel;
    }

    return nullptr;
}
