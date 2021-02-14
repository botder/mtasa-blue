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
#include "HTTPAuthRateLimit.h"
#include "HTTPGameMutex.h"
#include "HTTPAccountAuth.h"
#include "HTTPResourceRouter.h"
#include "ServerFactory.h"

namespace mtasa
{
    HTTPServerManager::HTTPServerManager()
    {
        m_verification = std::make_unique<HTTPServerVerification>();
        m_authRateLimit = std::make_unique<HTTPAuthRateLimit>();
        m_gameMutex = std::make_unique<HTTPGameMutex>();
        m_authentication = std::make_unique<HTTPAccountAuth>();
        m_resourceRouter = std::make_unique<HTTPResourceRouter>();
    }

    HTTPServerManager::~HTTPServerManager() = default;

    bool HTTPServerManager::Start(const char* hostname, unsigned short port)
    {
        if (m_httpServer)
            return false;

        m_httpServer = ServerFactory::CreateHTTPServer(hostname, port);

        if (m_httpServer == nullptr)
            return false;

        m_httpServer->AppendMiddleware(m_verification.get());
        m_httpServer->AppendMiddleware(m_authRateLimit.get());
        m_httpServer->AppendMiddleware(m_gameMutex.get());
        m_httpServer->AppendMiddleware(m_authentication.get());
        m_httpServer->AppendMiddleware(m_resourceRouter.get());

        return true;
    }

    void HTTPServerManager::Stop()
    {
        ServerFactory::DestroyServer(m_httpServer);
    }

    void HTTPServerManager::SetDefaultResource(const char* resourceName)
    {
        m_resourceRouter->SetDefaultResource(resourceName);
    }
}            // namespace mtasa
