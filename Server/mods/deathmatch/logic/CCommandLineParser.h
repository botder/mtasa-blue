/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CCommandLineParser.h
 *  PURPOSE:     Command line parser class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>

class CCommandLineParser
{
public:
    bool Parse(int iArgumentCount, char* szArguments[]);

    bool GetMainConfig(const char*& szMainConfig)
    {
        if (m_bMainConfig)
        {
            szMainConfig = m_strMainConfig.c_str();
            return true;
        }
        return false;
    }
    bool GetIP(std::string& strIP)
    {
        if (m_bIP)
        {
            strIP = m_strIP;
            return true;
        }
        return false;
    }
    bool GetDNS(std::string& out) const noexcept
    {
        if (m_usingDNS)
        {
            out = m_dns;
            return true;
        }
        return false;
    }
    bool GetIPv6Only(std::string& out) const noexcept
    {
        if (m_usingIpv6Only)
        {
            out = m_ipv6Only;
            return true;
        }
        return false;
    }
    bool GetPort(unsigned short& usPort)
    {
        if (m_bPort)
        {
            usPort = m_usPort;
            return true;
        }
        return false;
    }
    bool GetHTTPPort(unsigned short& usHTTPPort)
    {
        if (m_bHTTPPort)
        {
            usHTTPPort = m_usHTTPPort;
            return true;
        }
        return false;
    }
    bool GetMaxPlayers(unsigned int& uiMaxPlayers)
    {
        if (m_bMaxPlayers)
        {
            uiMaxPlayers = m_uiMaxPlayers;
            return true;
        }
        return false;
    }
    bool IsVoiceDisabled(bool& bDisabled)
    {
        if (m_bNoVoice)
        {
            bDisabled = m_bDisableVoice;
            return true;
        }
        return false;
    }

private:
    bool m_bMainConfig = false;
    bool m_bIP = false;
    bool m_usingDNS = false;
    bool m_usingIpv6Only = false;
    bool m_bPort = false;
    bool m_bHTTPPort = false;
    bool m_bMaxPlayers = false;
    bool m_bNoVoice = false;

    std::string    m_strMainConfig;
    std::string    m_strIP;
    std::string    m_dns;
    std::string    m_ipv6Only;
    unsigned short m_usPort = 0;
    unsigned short m_usHTTPPort = 0;
    unsigned int   m_uiMaxPlayers = 0;
    bool           m_bDisableVoice = false;
};
