/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource file router for embedded HTTP web server
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <ehs/ehs.h>

class CAccount;
class CResource;

namespace mtasa
{
    class ResourceFileRouter final : public EHS
    {
    public:
        ResourceFileRouter(CResource& resource);
        ~ResourceFileRouter();

        ResponseCode HandleRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse) override;

    private:
        ResponseCode HandleFileRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse, CAccount* account);
        ResponseCode HandleCallRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse, CAccount* account);

        bool IsHttpAccessAllowed(CAccount* account);

    private:
        CResource& m_resource;
    };
}
