/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     HTTP server authentication middleware
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "HTTPServer.h"

namespace mtasa
{
    class HTTPServerVerification : public HTTPMiddleware
    {
    public:
        bool PreProcessRequest(const HTTPServer& server, const HTTPRequest& request, HTTPResponse& response) override;
    };
}
