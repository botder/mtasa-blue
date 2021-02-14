/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Router to resources for HTTP requests
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "HTTPResourceRouter.h"

namespace mtasa
{
    bool HTTPResourceRouter::PreProcessRequest(const HTTPServer& server, const HTTPRequest& request, HTTPResponse& response)
    {
        if (!g_pGame->IsServerFullyUp())
        {
            response.statusCode = 200;
            response.body = "The server is not ready. Please try again in a minute.";
            return false;
        }

        g_pGame->Lock();
        response.statusCode = 200;
        response.body = "Hello, world!";
        return true;
    }

    void HTTPResourceRouter::PostProcessRequest(const HTTPServer& server, const HTTPRequest& request, HTTPResponse& response)
    {
        g_pGame->Unlock();
    }
}            // namespace mtasa
