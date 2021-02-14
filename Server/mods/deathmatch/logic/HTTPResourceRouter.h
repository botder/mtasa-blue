/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Router to resources for HTTP requests
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "HTTPServer.h"

namespace mtasa
{
    class HTTPResourceRouter : public HTTPMiddleware
    {
    public:
        bool PreProcessRequest(const HTTPServer& server, const HTTPRequest& request, HTTPResponse& response) override;
        void PostProcessRequest(const HTTPServer& server, const HTTPRequest& request, HTTPResponse& response) override;
    };
}
