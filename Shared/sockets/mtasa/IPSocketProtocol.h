/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPSocketProtocol.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace mtasa
{
    enum class IPSocketProtocol : std::uint8_t
    {
        Unspecified,
        TCP,
        UDP,
    };
}            // namespace mtasa
