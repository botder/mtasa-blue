/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Middleware for resource routing
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Middleware.h"

namespace mtasa
{
    class ResourceRouterMiddleware : public Middleware
    {
    public:
        bool PreProcessRequest(const web::Request& request, web::Response& response, AuxiliaryMiddlewarePayload& payload) override;

    public:
        void SetDefaultResource(std::string_view resourceName) { m_defaultResourceName = std::string(resourceName); }

    private:
        std::string m_defaultResourceName;
    };
}
