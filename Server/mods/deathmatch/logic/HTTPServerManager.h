/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     HTTP server manager and custom middleware organization
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <memory>

namespace mtasa
{
    class HTTPServer;
    class HTTPServerVerification;
    class HTTPResourceRouter;

    class HTTPServerManager
    {
    public:
        HTTPServerManager();
        ~HTTPServerManager();

        bool Start(const char* hostname, unsigned short port);
        void Stop();

    private:
        std::unique_ptr<HTTPServer>             m_httpServer;
        std::unique_ptr<HTTPServerVerification> m_verification;
        std::unique_ptr<HTTPResourceRouter>     m_resourceRouter;
    };
}            // namespace mtasa
