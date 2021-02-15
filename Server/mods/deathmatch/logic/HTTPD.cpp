/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Built-in HTTP webserver class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "HTTPD.h"
#include "web/Server.h"
#include "web/Request.h"
#include "web/Response.h"
#include "middleware/ServerVerificationMiddleware.h"
#include "middleware/GameMutexMiddleware.h"
#include "middleware/AccountAuthMiddleware.h"
#include "middleware/ResourceRouterMiddleware.h"

namespace mtasa
{
    using namespace web;

    HTTPD::HTTPD()
    {
        m_serverVerification = std::make_unique<ServerVerificationMiddleware>();
        m_gameMutex = std::make_unique<GameMutexMiddleware>();
        m_accountAuth = std::make_unique<AccountAuthMiddleware>();
        m_resourceRouter = std::make_unique<ResourceRouterMiddleware>();

        m_middlewares.reserve(4);
        m_middlewares.emplace_back(m_serverVerification.get());
        m_middlewares.emplace_back(m_gameMutex.get());
        m_middlewares.emplace_back(m_accountAuth.get());
        m_middlewares.emplace_back(m_resourceRouter.get());

        m_guestAccount = g_pGame->GetAccountManager()->AddGuestAccount(HTTP_GUEST_ACCOUNT_NAME);
    }

    HTTPD::~HTTPD() = default;

    bool HTTPD::IsRunning() const
    {
        return m_httpServer != nullptr && m_httpServer->IsRunning();
    }

    bool HTTPD::Start(std::string_view hostname, std::uint16_t port)
    {
        std::unique_ptr<Server> httpServer = Server::Create(Protocol::HTTP);

        if (httpServer == nullptr)
            return false;

        httpServer->SetRequestHandler([this](const Request& request, Response& response) { HandleRequest(request, response); });

        if (!httpServer->Start(hostname, port))
            return false;

        m_httpServer = std::move(httpServer);
        return true;
    }

    void HTTPD::Stop()
    {
        if (m_httpServer != nullptr)
        {
            m_httpServer->Stop();
            m_httpServer.reset();
        }
    }

    void HTTPD::SetDefaultResource(std::string_view resourceName)
    {
        m_resourceRouter->SetDefaultResource(resourceName);
    }

    void HTTPD::HandleRequest(const Request& request, Response& response)
    {
        AuxiliaryMiddlewarePayload payload;
        payload.account = m_guestAccount;

        std::size_t i = 0;

        for (; i < m_middlewares.size(); i++)
        {
            if (!m_middlewares[i]->PreProcessRequest(request, response, payload))
            {
                break;
            }
        }

        // The middleware, which aborted the processing, won't be called here
        for (; i-- > 0;)
        {
            m_middlewares[i]->PostProcessRequest(request, response, payload);
        }
    }
}
