/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     General purpose server factory
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Protocol.h"
#include <memory>
#include <cstdint>
#include <string_view>
#include <functional>

namespace mtasa::web
{
    class Response;
    class Request;

    class Server
    {
    public:
        using RequestHandler = std::function<void(const Request& request, Response& response)>;

    public:
        static void Initialize();
        static void Shutdown();

        static std::unique_ptr<Server> Create(Protocol protocol);

        static std::string Decode(std::string_view input, bool isURIParameter);

    public:
        virtual ~Server() = default;

    public:
        virtual bool IsRunning() const { return m_isRunning; }

        virtual bool Start(std::string_view hostname, std::uint16_t port) = 0;
        virtual void Stop() = 0;

        virtual bool SetCertificate(std::string_view absoluteFilePath, bool containsKey) { return false; }
        virtual bool SetCertificateKey(std::string_view absoluteFilePath) { return false; }

        virtual bool SetRequestHandler(RequestHandler handler) { return false; }

        virtual bool HasMaxHeaderSize() const { return false; }
        virtual bool SetMaxHeaderSize(std::size_t size) { return false; }
        virtual bool TryGetMaxHeaderSize(std::size_t& size) const { return false; }

    protected:
        bool m_isRunning = false;
    };
}            // namespace mtasa
