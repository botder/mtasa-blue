/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/SocketHandle.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace mtasa
{
#ifdef _WIN32
    using SocketHandle = std::uintptr_t;
#else
    using SocketHandle = int;
#endif

    static constexpr auto INVALID_SOCKET_HANDLE = static_cast<SocketHandle>(-1);
}            // namespace mtasa
