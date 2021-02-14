/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     HTTP authentication with our account manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "HTTPServer.h"

class CAccount;
class CAccountManager;

namespace mtasa
{
    class HTTPAccountAuth : public HTTPMiddleware
    {
    public:
        HTTPAccountAuth();

    public:
        bool PreProcessRequest(HTTPRequest& request, HTTPResponse& response) override;

    private:
        CAccountManager* m_accountManager = nullptr;
        CAccount*        m_guestAccount = nullptr;
    };
}            // namespace mtasa
