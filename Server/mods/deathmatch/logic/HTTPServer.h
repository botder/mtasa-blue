/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     HTTP server
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <vector>
#include <array>
#include <string_view>
#include <unordered_map>
#include <cstdint>

class CAccount;

namespace mtasa
{
    std::string httpDecode(std::string_view input, bool isURLParameter);

    class HTTPHeader
    {
    public:
        std::string_view name;
        std::string_view value;
    };

    class HTTPParameter
    {
    public:
        std::string_view name;
        std::string_view value;
    };

    class HTTPConnection
    {
    public:
        bool             isIPv6 = false;
        std::string_view ip;
        std::uint16_t    port = 0;
    };

    class HTTPAuthentication
    {
    public:
        std::string_view username;
        std::string_view password;
        CAccount*        account = nullptr; // for convenience
    };

    class HTTPRequest
    {
    public:
        static constexpr std::size_t MAX_HEADERS = 40;
        static constexpr std::size_t MAX_PARAMETERS = 25;

    public:
        bool TryGetHeader(std::string_view headerName, HTTPHeader& out) const noexcept
        {
            for (const HTTPHeader& header : headers)
            {
                if (header.name.empty())
                    break;

                if (header.name == headerName)
                {
                    out = header;
                    return true;
                }
            }

            return false;
        }

    public:
        HTTPConnection     connection;
        HTTPAuthentication auth;

        std::string_view protocol;
        std::string_view method;
        std::string_view uri;
        std::string_view query;
        std::string_view body;
        std::string_view host;

        std::array<HTTPParameter, MAX_PARAMETERS> parameters;
        std::array<HTTPHeader, MAX_HEADERS>       headers;
    };

    class HTTPResponse
    {
    public:
        static constexpr bool IsStandardStatusCode(int code) noexcept { return code >= 100 && code <= 599; }

    public:
        int                                          statusCode = 500;
        std::unordered_map<std::string, std::string> headers;
        std::string                                  body;
        bool                                         serveFile = false;
    };

    class HTTPServer;

    class HTTPMiddleware
    {
    public:
        virtual ~HTTPMiddleware() = default;
        virtual bool PreProcessRequest(HTTPRequest& request, HTTPResponse& response) { return true; }
        virtual void PostProcessRequest(HTTPRequest& request, HTTPResponse& response) {}
    };

    class HTTPServer
    {
    public:
        static constexpr std::size_t RESPONSE_BUFFER_PROVISION = 128;

    public:
        void  SetHandle(void* handle) noexcept { m_handle = handle; }
        void* GetHandle() const noexcept { return m_handle; }

        void        SetMaxHeaderSize(std::size_t size) noexcept { m_maxHeaderSize = size; }
        std::size_t GetMaxHeaderSize() const noexcept { return m_maxHeaderSize; }

        std::vector<char>& GetResponseBuffer() noexcept { return m_responseBuffer; }

        void AppendMiddleware(HTTPMiddleware* middleware)
        {
            if (middleware != nullptr)
                m_middlewares.emplace_back(middleware);
        }

        void RemoveMiddleware(HTTPMiddleware* middleware)
        {
            if (middleware != nullptr)
                m_middlewares.erase(std::find(m_middlewares.begin(), m_middlewares.end(), middleware));
        }

        void ProcessRequest(HTTPRequest& request, HTTPResponse& response)
        {
            std::size_t i = 0;

            for (; i < m_middlewares.size(); i++)
            {
                if (!m_middlewares[i]->PreProcessRequest(request, response))
                {
                    break;
                }
            }

            // The middleware, which aborted the processing, won't be called here
            for (; i-- > 0;)
            {
                m_middlewares[i]->PostProcessRequest(request, response);
            }
        }

    private:
        void*             m_handle = nullptr;
        std::vector<char> m_responseBuffer;
        std::size_t       m_maxHeaderSize = 8192;
        std::string       m_baseAddress;

        std::vector<HTTPMiddleware*> m_middlewares;
    };
}
