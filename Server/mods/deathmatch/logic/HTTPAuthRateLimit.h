/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     HTTP authentication rate limiting to prevent brute force attacks
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "HTTPServer.h"

namespace mtasa
{
    class HTTPAuthRateLimit : public HTTPMiddleware
    {
    public:
        bool PreProcessRequest(HTTPRequest& request, HTTPResponse& response) override;
        void PostProcessRequest(HTTPRequest& request, HTTPResponse& response) override;
    };
}            // namespace mtasa
