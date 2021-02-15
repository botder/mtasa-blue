/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Middleware interface for built-in HTTP webserver
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string_view>

class CAccount;

namespace mtasa
{
    namespace web
    {
        class Request;
        class Response;
    }

    class AuxiliaryMiddlewarePayload
    {
    public:
        CAccount*        account = nullptr;
        std::string_view relativeResourcePath;
    };

    class Middleware
    {
    public:
        virtual ~Middleware() = default;

    public:
        virtual bool PreProcessRequest(const web::Request& request, web::Response& response, AuxiliaryMiddlewarePayload& payload)
        {
            return true;
        }

        virtual void PostProcessRequest(const web::Request& request, web::Response& response, AuxiliaryMiddlewarePayload& payload)
        {
        }
    };
}
