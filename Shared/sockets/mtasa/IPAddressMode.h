/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPAddressMode.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace mtasa
{
    enum class IPAddressMode : std::uint8_t
    {
        IPv4Only,
        IPv6Only,
        IPv6DualStack,
    };
}            // namespace mtasa
