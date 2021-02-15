/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Web client
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <string_view>

namespace mtasa::web
{
    enum class InternetProtocol
    {
        V4,         // Version 4 (https://tools.ietf.org/html/rfc791)
        V6,         // Version 6 (https://tools.ietf.org/html/rfc8200)
    };

    class Client final
    {
    public:
        InternetProtocol protocol;
        std::string_view address;
        std::uint16_t    port;
    };
}            // namespace mtasa::web
