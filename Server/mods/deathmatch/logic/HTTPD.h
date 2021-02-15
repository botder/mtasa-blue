/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Built-in HTTP webserver class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <memory>
#include <cstdint>
#include <string_view>

namespace mtasa
{
    namespace web
    {
        class Server;
        class Request;
        class Response;
    }

    class Middleware;
    class ServerVerificationMiddleware;
    class GameMutexMiddleware;
    class AccountAuthMiddleware;
    class ResourceRouterMiddleware;

    class HTTPD final
    {
    public:
        HTTPD();
        ~HTTPD();

        bool IsRunning() const;

        bool Start(std::string_view hostname, std::uint16_t port);
        void Stop();

        void SetDefaultResource(std::string_view resourceName);

    private:
        void HandleRequest(const mtasa::web::Request& request, mtasa::web::Response& response);

    private:
        std::unique_ptr<web::Server> m_httpServer;
        std::vector<Middleware*>     m_middlewares;
        CAccount*                    m_guestAccount = nullptr;

        std::unique_ptr<ServerVerificationMiddleware> m_serverVerification;
        std::unique_ptr<GameMutexMiddleware>          m_gameMutex;
        std::unique_ptr<AccountAuthMiddleware>        m_accountAuth;
        std::unique_ptr<ResourceRouterMiddleware>     m_resourceRouter;
    };
}
