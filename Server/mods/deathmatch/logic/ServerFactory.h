/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     General purpose server manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <functional>
#include <string_view>
#include <array>
#include <unordered_map>

namespace mtasa
{
    class HTTPHeader
    {
    public:
        std::string_view name;
        std::string_view value;
    };

    class HTTPRequest
    {
    public:
        static constexpr std::size_t MAX_HEADERS = 40;

    public:
        std::string_view protocol;
        std::string_view method;
        std::string_view uri;
        std::string_view query;
        std::string_view body;

        std::array<HTTPHeader, MAX_HEADERS> headers;
    };

    class HTTPResponse
    {
    public:
        static constexpr bool IsStandardStatusCode(int code) { return code >= 100 && code <= 599; }

    public:
        int                                          statusCode = 500;
        std::unordered_map<std::string, std::string> headers;
        std::string                                  body;
    };

    class HTTPServer
    {
    public:
        static constexpr std::size_t RESPONSE_BUFFER_PROVISION = 128;

        using RequestHandler = std::function<void(const HTTPRequest&, HTTPResponse&)>;

    public:
        void  SetHandle(void* handle) noexcept { m_handle = handle; }
        void* GetHandle() const noexcept { return m_handle; }

        void                  SetRequestHandler(RequestHandler handler) noexcept { m_requestHandler = handler; }
        const RequestHandler& GetRequestHandler() const noexcept { return m_requestHandler; }

        void        SetMaxHeaderSize(std::size_t size) noexcept { m_maxHeaderSize = size; }
        std::size_t GetMaxHeaderSize() const noexcept { return m_maxHeaderSize; }

        std::vector<char>& GetResponseBuffer() noexcept { return m_responseBuffer; }

    private:
        void*             m_handle = nullptr;
        RequestHandler    m_requestHandler;
        std::vector<char> m_responseBuffer;
        std::size_t       m_maxHeaderSize = 8192;
    };

    class ServerFactory
    {
    public:
        static void Initialize();
        static void Shutdown();

        static std::unique_ptr<HTTPServer> CreateHTTPServer(const char* hostname, unsigned short port);
        static void                        DestroyServer(std::unique_ptr<HTTPServer>& server);

    private:
        static std::atomic_bool ms_isRunning;
        static std::thread      ms_worker;
    };
}            // namespace mtasa
