/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Middleware for account authentication with MTA server accounts
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Middleware.h"

namespace mtasa
{
    class AccountAuthMiddleware : public Middleware
    {
    public:
        bool PreProcessRequest(const web::Request& request, web::Response& response, AuxiliaryMiddlewarePayload& payload) override;
    };
}
