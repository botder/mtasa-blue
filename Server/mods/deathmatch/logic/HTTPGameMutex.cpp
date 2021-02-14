/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     HTTP middleware for mutual exclusion of CGame
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "HTTPGameMutex.h"

namespace mtasa
{
    bool HTTPGameMutex::PreProcessRequest(HTTPRequest& request, HTTPResponse& response)
    {
        if (!g_pGame->IsServerFullyUp())
        {
            response.statusCode = 200;
            response.body = "The server is not ready. Please try again in a minute.";
            return false;
        }

        g_pGame->Lock();
        return true;
    }

    void HTTPGameMutex::PostProcessRequest(HTTPRequest& request, HTTPResponse& response)
    {
        g_pGame->Unlock();
    }
}            // namespace mtasa
