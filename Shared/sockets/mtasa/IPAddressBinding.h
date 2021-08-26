/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPAddressBinding.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "IPAddressMode.h"
#include "IPAddress.h"

namespace mtasa
{
    struct IPAddressBinding
    {
        IPAddress     address;
        IPAddressMode addressMode;

        IPAddressBinding(const IPAddress& address_, IPAddressMode addressMode_) : address(address_), addressMode(addressMode_) {}
    };
}            // namespace mtasa
