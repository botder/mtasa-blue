/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPAddressFamily.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace mtasa
{
    enum class IPAddressFamily : std::uint8_t
    {
        Unspecified,
        IPv4,
        IPv6,
    };
}
