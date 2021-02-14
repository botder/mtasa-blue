/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     HTTP server
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "HTTPServer.h"

#ifdef _CRT_SECURE_NO_WARNINGS
    #undef _CRT_SECURE_NO_WARNINGS // redefined in mongoose.h
#endif

extern "C"
{
    #include <mongoose.h>
}

namespace mtasa
{
    std::string httpDecode(std::string_view input, bool isURLParameter)
    {
        std::string buffer(input.size(), '\0');

        int length = mg_url_decode(input.data(), input.size(), buffer.data(), buffer.capacity() + 1, isURLParameter ? 1 : 0);

        if (length < 0)
            return std::string{};

        buffer.shrink_to_fit();
        return std::move(buffer);
    }
}
