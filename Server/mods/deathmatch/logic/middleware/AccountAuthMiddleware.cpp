/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Middleware for account authentication with MTA server accounts
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "AccountAuthMiddleware.h"
#include "web/Client.h"
#include "web/Request.h"
#include "web/Response.h"

namespace mtasa
{
    using namespace mtasa::web;

    bool AccountAuthMiddleware::PreProcessRequest(const web::Request& request, web::Response& response, AuxiliaryMiddlewarePayload& payload)
    {
        if (const Credentials* credentials = request.GetCredentials(); credentials != nullptr)
        {
            if (credentials->username.empty())
                return true;

            if (credentials->username == CONSOLE_ACCOUNT_NAME)
            {
                payload.account = nullptr;
            }
            else
            {
                std::string username(credentials->username);
                payload.account = g_pGame->GetAccountManager()->Get(username.c_str());
            }

            if (payload.account == nullptr)
            {
                response.SetStatusCode(401);
                return false;
            }

            bool skipIPVerification = true;

            if (!payload.account->IsPassword(std::string(credentials->password), &skipIPVerification))
            {
                payload.account = nullptr;
                response.SetStatusCode(401);
                return false;
            }

            std::string ip(request.GetClient().address);

            if (!skipIPVerification && !g_pGame->GetAccountManager()->IsHttpLoginAllowed(payload.account, ip))
            {
                payload.account = nullptr;
                response.SetStatusCode(401);
                return false;
            }

            payload.account->OnLoginHttpSuccess(ip);
        }

        return true;
    }
}
