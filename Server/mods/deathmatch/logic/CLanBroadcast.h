/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CLanBroadcast.h
 *  PURPOSE:     LAN server broadcasting class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <Common.h>
#include <string>
#include <mtasa/IPSocket.h>
#include <mtasa/IPAddressMode.h>

class CLanBroadcast final
{
public:
    CLanBroadcast(unsigned short usServerPort, mtasa::IPAddressMode addressMode);

    void DoPulse();

    unsigned short GetPort() { return SERVER_LIST_BROADCAST_PORT; };

private:
    bool CreateIPv6Socket(bool isIPv6Only);
    bool CreateIPv4Socket();

private:
    mtasa::IPSocket m_socket;

    std::string m_strClientMessage;
    std::string m_strServerMessage;
};
