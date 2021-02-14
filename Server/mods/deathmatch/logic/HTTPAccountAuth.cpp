/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     HTTP authentication with our account manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "HTTPAccountAuth.h"
#include "CAccountManager.h"

namespace mtasa
{
    HTTPAccountAuth::HTTPAccountAuth()
    {
        m_accountManager = g_pGame->GetAccountManager();
        m_guestAccount = m_accountManager->AddGuestAccount(HTTP_GUEST_ACCOUNT_NAME);
    }

    bool HTTPAccountAuth::PreProcessRequest(HTTPRequest& request, HTTPResponse& response)
    {
        if (request.auth.username.empty())
        {
            request.auth.account = m_guestAccount;
            return true;
        }

        if (request.auth.username == CONSOLE_ACCOUNT_NAME)
        {
            response.statusCode = 401;
            return false;
        }

        CAccount* account = m_accountManager->Get(std::string(request.auth.username).c_str());

        if (account == nullptr)
        {
            response.statusCode = 401;
            return false;
        }

        bool skipIPVerification = true;

        if (!account->IsPassword(std::string(request.auth.password), &skipIPVerification))
        {
            response.statusCode = 401;
            return false;
        }

        std::string ip(request.connection.ip);

        if (!skipIPVerification && !m_accountManager->IsHttpLoginAllowed(account, ip))
        {
            response.statusCode = 401;
            return false;
        }

        account->OnLoginHttpSuccess(ip);

        request.auth.account = account;
        return true;
    }
}            // namespace mtasa
