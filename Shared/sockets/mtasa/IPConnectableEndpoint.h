/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPConnectableEndpoint.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "IPEndpoint.h"

namespace mtasa
{
    struct IPConnectableEndpoint final
    {
        // Internet Protocol endpoint (address + port) to connect to.
        IPEndpoint endpoint{};

        IPConnectableEndpoint() noexcept = default;

        explicit IPConnectableEndpoint(const IPEndpoint& endpoint_) noexcept : endpoint{endpoint_} {}

        explicit IPConnectableEndpoint(const IPAddress& address, std::uint16_t port) noexcept : endpoint{address, port} {}
    };
}            // namespace mtasa
