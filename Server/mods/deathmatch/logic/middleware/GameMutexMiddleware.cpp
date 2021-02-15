/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Middleware for CGame mutex (un-)locking
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "GameMutexMiddleware.h"
#include "web/Response.h"

namespace mtasa
{
    using namespace mtasa::web;
    using namespace std::string_view_literals;

    bool GameMutexMiddleware::PreProcessRequest(const Request& request, Response& response, AuxiliaryMiddlewarePayload& payload)
    {
        if (!g_pGame->IsServerFullyUp())
        {
            response.SetStatusCode(200);
            response.SetBody("The server is not ready. Please try again in a minute."sv);
            return false;
        }

        g_pGame->Lock();
        return true;
    }

    void GameMutexMiddleware::PostProcessRequest(const Request& request, Response& response, AuxiliaryMiddlewarePayload& payload)
    {
        g_pGame->Unlock();
    }
}
