/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     HTTP server manager and custom middleware organization
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "HTTPServerManager.h"
#include "HTTPServer.h"
#include "HTTPServerVerification.h"
#include "HTTPResourceRouter.h"
#include "ServerFactory.h"

namespace mtasa
{
    HTTPServerManager::HTTPServerManager() = default;

    HTTPServerManager::~HTTPServerManager() = default;

    bool HTTPServerManager::Start(const char* hostname, unsigned short port)
    {
        if (m_httpServer)
            return false;

        m_httpServer = ServerFactory::CreateHTTPServer(hostname, port);

        if (m_httpServer == nullptr)
            return false;

        if (!m_verification)
            m_verification = std::make_unique<HTTPServerVerification>();

        m_httpServer->AppendMiddleware(m_verification.get());

        if (!m_resourceRouter)
            m_resourceRouter = std::make_unique<HTTPResourceRouter>();

        m_httpServer->AppendMiddleware(m_resourceRouter.get());

        return true;
    }

    void HTTPServerManager::Stop()
    {
        ServerFactory::DestroyServer(m_httpServer);
    }
}            // namespace mtasa
