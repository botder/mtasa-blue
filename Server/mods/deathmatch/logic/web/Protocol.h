/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Enumeration of web server protocols
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

namespace mtasa::web
{
    enum class Protocol
    {
        UNKNOWN,
        TCP,              // Transmission Control Protocol https://tools.ietf.org/html/rfc793
        UDP,              // User Datagram Protocol (https://tools.ietf.org/html/rfc768)
        HTTP,             // Hypertext Transfer Protocol (https://tools.ietf.org/html/rfc2616)
        HTTPS,            // HTTP over TLS (https://tools.ietf.org/html/rfc2818)
        WS,               // WebSocket over HTTP (https://tools.ietf.org/html/rfc6455)
        WSS,              // WebSocket over HTTPS (https://tools.ietf.org/html/rfc6455)
    };
}
