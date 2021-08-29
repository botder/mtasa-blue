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

        explicit IPConnectableEndpoint(const IPEndpoint& endpoint_) : endpoint(endpoint_) {}
    };
}            // namespace mtasa
