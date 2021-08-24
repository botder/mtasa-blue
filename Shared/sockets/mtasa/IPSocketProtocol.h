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

#include <utility>

namespace mtasa
{
    enum class IPSocketProtocol
    {
        Unspecified,
        TCP,
        UDP,
    };

    std::pair<int, int> TranslateSocketProtocol(IPSocketProtocol protocol);
}            // namespace mtasa
