/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     HTTP authentication rate limiting to prevent brute force attacks
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "HTTPAuthRateLimit.h"

namespace mtasa
{
    bool HTTPAuthRateLimit::PreProcessRequest(HTTPRequest& request, HTTPResponse& response)
    {
        if (request.auth.username.empty())
            return true;

        return true;
    }

    void HTTPAuthRateLimit::PostProcessRequest(HTTPRequest& request, HTTPResponse& response)
    {
    }
}            // namespace mtasa
