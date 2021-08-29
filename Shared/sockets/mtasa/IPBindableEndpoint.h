/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/sockets/mtasa/IPBindableEndpoint.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "IPEndpoint.h"

namespace mtasa
{
    struct IPBindableEndpoint final
    {
        // Internet Protocol endpoint (address + port) to listen on.
        IPEndpoint endpoint{};

        // If set to true, the bound socket will also accept IPv4 connections
        // on an unspecified IPv6 address.
        bool useDualMode = false;

        IPBindableEndpoint(const IPEndpoint& endpoint_, bool useDualMode_) : endpoint(endpoint_), useDualMode(useDualMode_) {}
    };
}            // namespace mtasa
