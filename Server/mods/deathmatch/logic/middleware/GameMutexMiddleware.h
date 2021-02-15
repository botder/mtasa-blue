/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Middleware for CGame mutex (un-)locking
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Middleware.h"

namespace mtasa
{
    class GameMutexMiddleware : public Middleware
    {
    public:
        bool PreProcessRequest(const web::Request& request, web::Response& response, AuxiliaryMiddlewarePayload& payload) override;
        void PostProcessRequest(const web::Request& request, web::Response& response, AuxiliaryMiddlewarePayload& payload) override;
    };
}
