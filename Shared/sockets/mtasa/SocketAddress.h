/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/SocketAddress.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <array>

namespace mtasa
{
    struct alignas(8) SocketAddress
    {
        std::array<std::uint8_t, 128> bytes;
    };

    static_assert(sizeof(SocketAddress) == 128, "Invalid size of SocketAddress");
}            // namespace mtasa
