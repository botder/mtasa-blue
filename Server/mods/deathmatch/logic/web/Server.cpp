/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     General purpose server factory
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "Server.h"
#include "Client.h"
#include "Request.h"
#include "Response.h"
#include <sstream>
#include <unordered_map>
#include <string.h>

#ifdef _CRT_SECURE_NO_WARNINGS
    #undef _CRT_SECURE_NO_WARNINGS // redefined in mongoose.h
#endif

extern "C"
{
    #include <mongoose.h>
}

using namespace std::string_view_literals;

namespace mtasa::web
{
    static struct Mongoose
    {
        mg_mgr           manager{};
        std::thread      worker;
        std::atomic_bool isRunning;
    } mongoose;

    constexpr bool iequals(const std::string_view& lhs, const std::string_view& rhs)
    {
        if (lhs.size() != rhs.size())
            return false;

        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](char a, char b) { return tolower(a) == tolower(b); });
    }

    struct iequal_to
    {
        bool operator()(const std::string& lhs, const std::string& rhs) const
        {
            if (lhs.size() != rhs.size())
                return false;

            return std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](char a, char b) { return tolower(a) == tolower(b); });
        }
    };

    class HTTPRequest : public Request
    {
    public:
        HTTPRequest() { m_protocol = Protocol::HTTP; }

        Client&       GetClient() override { return client; }
        const Client& GetClient() const override { return client; }

        std::string_view GetBody() const override { return body; }

        bool               HasCredentials() const override { return true; }
        Credentials*       GetCredentials() override { return &credentials; }
        const Credentials* GetCredentials() const override { return &credentials; }

        bool       HasURI() const override { return true; }
        URI*       GetURI() override { return &uri; }
        const URI* GetURI() const override { return &uri; }

        bool             HasMethod() const { return true; }
        std::string_view GetMethod() const { return method; }

        bool                       HasHeaders() const override { return true; }
        const std::vector<Header>* GetHeaders() const override { return &headers; }

    public:
        Client              client;
        Credentials         credentials;
        URI                 uri;
        std::string_view    method;
        std::string_view    body;
        std::vector<Header> headers;
    };

    class HTTPResponse final : public Response
    {
    public:
        static constexpr bool IsStandardStatusCode(int code) noexcept { return code >= 100 && code <= 599; }

    public:
        HTTPResponse() { SetMime("text/html"sv); }

    public:
        bool HasStatusCode() const override { return true; }

        bool SetStatusCode(int code) override
        {
            statusCode = code;
            return false;
        }

        bool TryGetStatusCode(int& code) override
        {
            code = statusCode;
            return true;
        }

        bool HasHeaders() const override { return true; }

        bool SetHeader(std::string_view name, std::string_view value) override
        {
            if (name.empty())
                return false;

            if (iequals("Content-Length"sv, name))
                return false;

            if (iequals("Content-Type"sv, name))
            {
                SetMime(value);
                return true;
            }

            headers[std::string(name)] = std::string(value);
            return true;
        }

    public:
        int statusCode = 500;

        std::unordered_map<std::string, std::string, std::hash<std::string>, iequal_to> headers;
    };

    class HTTPServer : public Server
    {
    public:
        static constexpr std::size_t RESPONSE_BUFFER_PROVISION = 128;

    public:
        bool Start(std::string_view hostname, std::uint16_t port) override
        {
            if (m_isRunning)
                return false;

            std::stringstream url;
            url << "http://" << hostname << ":" << port;

            mg_connection* c = mg_http_listen(&mongoose.manager, url.str().c_str(), &ProcessEvent, this);

            if (c == nullptr)
            {
                return false;
            }
            else if (!c->is_listening)
            {
                c->is_closing = 1;
                return false;
            }

            m_connection = c;
            m_isRunning = true;
            return true;
        }

        void Stop() override
        {
            if (!m_isRunning)
                return;

            m_connection->is_closing = 1;
            m_connection = nullptr;

            m_isRunning = false;
        }

        bool SetRequestHandler(RequestHandler handler) override
        {
            m_requestHandler = handler;
            return true;
        }

        bool HasMaxHeaderSize() const override { return true; }

        bool SetMaxHeaderSize(std::size_t size) override
        {
            m_maxHeaderSize = size;
            return true;
        }

        bool TryGetMaxHeaderSize(std::size_t& size) const override
        {
            size = m_maxHeaderSize;
            return true;
        }

    private:
        void ProcessMessage(mg_connection* connection, mg_http_message* message)
        {
            if (!m_requestHandler)
            {
                mg_http_reply(connection, 500, nullptr, "");
                CLogger::LogPrint("HTTP server error: request handler is unset\n");
                return;
            }

            HTTPRequest request;

            // NOTE(botder): The message parser always writes a reply on failure
            if (!ParseMessage(request, connection, message))
                return;

            HTTPResponse response;
            m_requestHandler(request, response);

            WriteResponse(connection, message, response);
        }

        bool ReserveBufferSpace()
        {
            if (m_responseBuffer.capacity() < (RESPONSE_BUFFER_PROVISION + m_maxHeaderSize))
            {
                m_responseBuffer.reserve(RESPONSE_BUFFER_PROVISION + m_maxHeaderSize);
            }

            return true;
        }

        void WriteResponse(mg_connection* connection, mg_http_message* message, const HTTPResponse& response)
        {
            if (!ReserveBufferSpace())
            {
                mg_http_reply(connection, 500, nullptr, "");
                CLogger::LogPrint("HTTP server error: failed to allocate response buffer\n");
                return;
            }

            if (!HTTPResponse::IsStandardStatusCode(response.statusCode))
            {
                mg_http_reply(connection, 500, nullptr, "");
                CLogger::LogPrintf("HTTP server error: non-standard response status code %d\n", response.statusCode);
                return;
            }

            const std::string& body = response.GetBody();
            const std::string& mime = response.GetMime();

            m_buffer = m_responseBuffer.data();
            m_bufferSize = m_responseBuffer.capacity();

            if (response.IsBodyFilePath())
            {
                if (body.empty())
                {
                    mg_http_reply(connection, 500, nullptr, "");
                    return;
                }

                // NOTE(botder): We need the trailing null terminator byte for `mg_http_serve_file` below (m_bufferSize must be 1 or higher)
                if (!WriteHeadersToBuffer(response) || m_bufferSize < 1)
                {
                    mg_http_reply(connection, 500, nullptr, "");
                    CLogger::LogPrint("HTTP server error: failed to write response headers\n");
                    return;
                }

                m_buffer[0] = '\0';
                mg_http_serve_file(connection, message, body.c_str(), mime.c_str(), m_responseBuffer.data());
            }
            else
            {
                if (!WriteInitialResponseLineToBuffer(response))
                {
                    mg_http_reply(connection, 500, nullptr, "");
                    CLogger::LogPrint("HTTP server error: failed to write initial response line\n");
                    return;
                }

                if (!WriteHeaderToBuffer("Content-Type"sv, mime))
                {
                    mg_http_reply(connection, 500, nullptr, "");
                    CLogger::LogPrint("HTTP server error: failed to write response content type\n");
                    return;
                }

                if (!WriteHeaderToBuffer("Content-Length"sv, std::to_string(body.size())))
                {
                    mg_http_reply(connection, 500, nullptr, "");
                    CLogger::LogPrint("HTTP server error: failed to write response content type\n");
                    return;
                }

                if (!WriteHeadersToBuffer(response))
                {
                    mg_http_reply(connection, 500, nullptr, "");
                    CLogger::LogPrint("HTTP server error: failed to write response headers\n");
                    return;
                }

                if (m_bufferSize < 2)
                {
                    mg_http_reply(connection, 500, nullptr, "");
                    CLogger::LogPrint("HTTP server error: not enough space for header terminator\n");
                    return;
                }

                memcpy(m_buffer, "\r\n", 2);
                m_buffer += 2;

                // Send response
                mg_send(connection, m_responseBuffer.data(), m_buffer - m_responseBuffer.data());

                if (!body.empty())
                {
                    mg_send(connection, body.c_str(), body.size());
                }
            }
        }

        bool WriteInitialResponseLineToBuffer(const HTTPResponse& response)
        {
            int bytesWritten = snprintf(m_buffer, m_bufferSize, "HTTP/1.1 %d OK\r\n", response.statusCode);

            if (bytesWritten <= 0)
                return false;

            m_buffer += bytesWritten;
            m_bufferSize -= bytesWritten;
            return true;
        }

        bool WriteHeadersToBuffer(const HTTPResponse& response)
        {
            if (response.headers.empty())
                return true;

            if (m_buffer == nullptr || m_bufferSize < 4)
                return false;

            for (const auto& [name, value] : response.headers)
            {
                if (!WriteHeaderToBuffer(name, value))
                    return false;
            }

            return true;
        }

        bool WriteHeaderToBuffer(std::string_view name, std::string_view value)
        {
            if (m_buffer == nullptr)
                return false;

            if (name.empty() || value.empty())
                return true;

            // Format: 'Name: Value\r\n'
            std::size_t requiredBufferSize = name.size() + 2 + value.size() + 2;

            if (requiredBufferSize > m_bufferSize)
                return false;

            memcpy(m_buffer, name.data(), name.size());
            m_buffer += name.size();

            memcpy(m_buffer, ": ", 2);
            m_buffer += 2;

            memcpy(m_buffer, value.data(), value.size());
            m_buffer += value.size();

            memcpy(m_buffer, "\r\n", 2);
            m_buffer += 2;

            return true;
        }

    private:
        static bool ParseMessage(HTTPRequest& request, mg_connection* connection, mg_http_message* message)
        {
            // Fill request client
            if (connection->peer.is_ip6)
            {
                request.client.protocol = InternetProtocol::V6;
                request.client.address = std::string_view(reinterpret_cast<const char*>(&connection->peer.ip6), 16);
                request.client.port = connection->peer.port;
            }
            else
            {
                request.client.protocol = InternetProtocol::V4;
                request.client.address = std::string_view(reinterpret_cast<const char*>(&connection->peer.ip), 4);
                request.client.port = connection->peer.port;
            }

            // Fill request method
            request.method = std::string_view(message->method.ptr, message->method.len);

            // Fill request body
            request.body = std::string_view(message->body.ptr, message->body.len);

            // Fill request credentials
            static char username[256];
            static char password[256];
            mg_http_creds(message, username, std::size(username), password, std::size(password));
            request.credentials.username = std::string_view(username);
            request.credentials.password = std::string_view(password);

            // Fill request uri
            request.uri.scheme = "http"sv;
            request.uri.path = std::string_view(message->uri.ptr, message->uri.len);
            request.uri.query = std::string_view(message->query.ptr, message->query.len);

            if (struct mg_str* host = mg_http_get_header(message, "Host"); host == nullptr)
            {
                // A `Host` header field must be sent in all HTTP/1.1 request messages.
                // Responding with a `400 (Bad Request)` is permitted in that case.
                mg_http_reply(connection, 400, nullptr, "");
                return false;
            }
            else
            {
                std::string_view hostname(host->ptr, host->len);
                request.uri.hostname = hostname.substr(0, hostname.find(':'));

                if (request.uri.hostname.size() < hostname.size())
                {
                    request.uri.port = hostname.substr(request.uri.hostname.size() + 1);
                }

                if (request.uri.hostname.empty())
                {
                    mg_http_reply(connection, 400, nullptr, "");
                    return false;
                }
            }

            // Fill request headers
            request.headers.reserve(MG_MAX_HTTP_HEADERS);

            for (std::size_t i = 0; i < MG_MAX_HTTP_HEADERS; i++)
            {
                mg_http_header& header = message->headers[i];

                if (header.name.ptr == nullptr)
                    break;

                Header entry;
                entry.name = std::string_view(header.name.ptr, header.name.len);
                entry.value = std::string_view(header.value.ptr, header.value.len);
                request.headers.emplace_back(std::move(entry));
            }

            return true;
        }
        
        static void ProcessEvent(mg_connection* connection, int event, void* eventdata, void* userdata)
        {
            auto server = reinterpret_cast<HTTPServer*>(userdata);

            if (server == nullptr)
            {
                connection->is_closing = 1;
                return;
            }

            if (event == MG_EV_ERROR)
            {
                auto error = reinterpret_cast<const char*>(eventdata);
                CLogger::LogPrintf("HTTP server error: %s\n", error);
                return;
            }

            if (event != MG_EV_HTTP_MSG)
                return;

            auto message = reinterpret_cast<mg_http_message*>(eventdata);
            server->ProcessMessage(connection, message);
        }

    private:
        mg_connection*    m_connection = nullptr;
        RequestHandler    m_requestHandler;
        std::vector<char> m_responseBuffer;
        std::size_t       m_maxHeaderSize = 8192;
        char*             m_buffer = nullptr;
        std::size_t       m_bufferSize = 0;
    };

    void Server::Initialize()
    {
        if (mongoose.isRunning.exchange(true))
            return;

        mg_mgr_init(&mongoose.manager);

        mongoose.worker = std::thread([] {
            while (mongoose.isRunning.load())
            {
                mg_mgr_poll(&mongoose.manager, 50);
                mg_usleep(5'000);
            }
        });
    }

    void Server::Shutdown()
    {
        if (!mongoose.isRunning.exchange(false))
            return;

        if (mongoose.worker.joinable())
            mongoose.worker.join();

        mg_mgr_free(&mongoose.manager);
    }

    std::unique_ptr<Server> Server::Create(Protocol protocol)
    {
        switch (protocol)
        {
            case Protocol::HTTP:
                return std::unique_ptr<Server>(new HTTPServer());
            default:
                break;
        }

        return nullptr;
    }

    std::string Server::Decode(std::string_view input, bool isURIParameter)
    {
        std::string buffer(input.size(), '\0');

        int length = mg_url_decode(input.data(), input.size(), buffer.data(), buffer.size() + 1, isURIParameter ? 1 : 0);

        if (length < 0)
            return std::string{};

        buffer.shrink_to_fit();
        return std::move(buffer);
    }
}
