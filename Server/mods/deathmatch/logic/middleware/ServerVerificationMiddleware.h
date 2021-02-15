/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Middleware for MTA server verification
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Middleware.h"

namespace mtasa
{
    class ServerVerificationMiddleware : public Middleware
    {
    public:
        bool PreProcessRequest(const web::Request& request, web::Response& response, AuxiliaryMiddlewarePayload& payload) override;
    };
}
