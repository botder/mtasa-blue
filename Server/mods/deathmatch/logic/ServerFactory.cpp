/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     General purpose server manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "ServerFactory.h"
#include "HTTPServer.h"
#include "CLogger.h"
#include <sstream>
#include <stdio.h>

#ifdef _CRT_SECURE_NO_WARNINGS
    #undef _CRT_SECURE_NO_WARNINGS // redefined in mongoose.h
#endif

extern "C"
{
    #include <mongoose.h>
}

namespace mtasa
{
    static mg_mgr mongooseManager{};

    std::atomic_bool ServerFactory::ms_isRunning;
    std::thread      ServerFactory::ms_worker;

    void MongooseHTTPServerCallback(mg_connection* connection, int event, void* eventdata, void* userdata);

    void ServerFactory::Initialize()
    {
        if (ms_isRunning.exchange(true))
            return;

        mg_mgr_init(&mongooseManager);

        ms_worker = std::thread([] {
            while (ms_isRunning.load())
            {
                mg_mgr_poll(&mongooseManager, 50);
                mg_usleep(5'000);
            }
        });
    }

    void ServerFactory::Shutdown()
    {
        if (!ms_isRunning.exchange(false))
            return;

        if (ms_worker.joinable())
            ms_worker.join();

        mg_mgr_free(&mongooseManager);
    }

    std::unique_ptr<HTTPServer> ServerFactory::CreateHTTPServer(const char* hostname, unsigned short port)
    {
        if (!ms_isRunning.load())
            return nullptr;

        std::stringstream address;
        address << "http://" << hostname << ":" << port;

        auto server = std::make_unique<HTTPServer>();
        
        mg_connection* connection = mg_http_listen(&mongooseManager, address.str().c_str(), &MongooseHTTPServerCallback, server.get());
        server->SetHandle(connection);

        return std::move(server);
    }

    void ServerFactory::DestroyServer(std::unique_ptr<HTTPServer>& server)
    {
        if (!ms_isRunning.load())
            return;

        auto connection = reinterpret_cast<mg_connection*>(server->GetHandle());

        if (connection != nullptr)
            connection->is_closing = 1;

        server.reset();
    }

    void MongooseHTTPServerCallback(mg_connection* connection, int event, void* eventdata, void* userdata)
    {
        auto server = reinterpret_cast<HTTPServer*>(userdata);

        if (event == MG_EV_ERROR)
        {
            auto error = reinterpret_cast<const char*>(eventdata);
            CLogger::LogPrintf("HTTP server error: %s", error);
            return;
        }

        if (event != MG_EV_HTTP_MSG)
            return;

        auto message = reinterpret_cast<mg_http_message*>(eventdata);

        HTTPRequest request;
        request.connection.isIPv6 = connection->peer.is_ip6;
        request.connection.port = connection->peer.port;

        if (connection->peer.is_ip6)
        {
            request.connection.ip = std::string_view(reinterpret_cast<const char*>(&connection->peer.ip6), 16);
        }
        else
        {
            request.connection.ip = std::string_view(reinterpret_cast<const char*>(&connection->peer.ip), 4);
        }

        char username[256];
        char password[256];
        mg_http_creds(message, username, std::size(username), password, std::size(password));
        request.auth.username = std::string_view(username);
        request.auth.password = std::string_view(password);
        
        request.protocol = std::string_view(message->proto.ptr, message->proto.len);
        request.method = std::string_view(message->method.ptr, message->method.len);
        request.uri = std::string_view(message->uri.ptr, message->uri.len);
        request.query = std::string_view(message->query.ptr, message->query.len);
        request.body = std::string_view(message->body.ptr, message->body.len);

        for (std::size_t i = 0; i < std::min<std::size_t>(HTTPRequest::MAX_HEADERS, MG_MAX_HTTP_HEADERS); i++)
        {
            mg_http_header& header = message->headers[i];

            if (header.name.ptr == nullptr)
                break;

            request.headers[i] = HTTPHeader{std::string_view(header.name.ptr, header.name.len), std::string_view(header.value.ptr, header.value.len)};

            if (request.headers[i].name == "Host")
                request.host = request.headers[i].value;
        }

        if (request.host.empty())
        {
            // A `Host` header field must be sent in all HTTP/1.1 request messages.
            // Responding with a `400 (Bad Request)` is permitted in that case.
            mg_http_reply(connection, 400, nullptr, "");
            return;
        }

        if (!request.query.empty())
        {
            std::size_t i = 0;
            std::size_t offset = 0;

            while (i < HTTPRequest::MAX_PARAMETERS && offset != std::string_view::npos)
            {
                std::size_t      ampersand = request.query.find('&', offset);
                std::string_view parameter = request.query.substr(0, ampersand);

                if (!parameter.empty())
                {
                    std::size_t equals = parameter.find('=');

                    if (equals > 0)
                    {
                        request.parameters[i].name = parameter.substr(0, equals);
                        request.parameters[i].value = parameter.substr(equals + 1);
                        i++;
                    }
                }

                offset = (ampersand == std::string_view::npos) ? ampersand : (ampersand + 1);
            }
        }

        HTTPResponse response;
        server->ProcessRequest(request, response);

        if (!HTTPResponse::IsStandardStatusCode(response.statusCode))
        {
            mg_http_reply(connection, 500, nullptr, "Internal Server Error");
            CLogger::LogPrintf("HTTP server error: non-standard response status code %d", response.statusCode);
            return;
        }

        if (response.serveFile)
        {
            const char* mime = "application/octet-stream";

            if (auto iter = response.headers.find("Content-Type"); iter != response.headers.end())
                mime = iter->second.c_str();

            mg_http_serve_file(connection, message, response.body.c_str(), mime, nullptr);
            return;
        }

        // Add 'Content-Length' header
        response.headers.try_emplace("Content-Length", std::to_string(response.body.size()));

        // Allocate or resize response buffer
        std::vector<char>& buffer = server->GetResponseBuffer();
        std::size_t maxHeaderSize = server->GetMaxHeaderSize();
        std::size_t remainingBufferSize = buffer.capacity();
        std::size_t bufferOffset = 0;

        if (remainingBufferSize < (HTTPServer::RESPONSE_BUFFER_PROVISION + maxHeaderSize))
        {
            buffer.reserve(HTTPServer::RESPONSE_BUFFER_PROVISION + maxHeaderSize);
            remainingBufferSize = buffer.capacity();
        }

        // Write initial response line
        int offset = snprintf(buffer.data(), buffer.capacity(), "HTTP/1.1 %d OK\r\n", response.statusCode);
        
        if (offset < 0)
        {
            mg_http_reply(connection, 500, nullptr, "Internal Server Error");
            CLogger::LogPrint("HTTP server error: failed to write initial response line");
            return;
        }

        bufferOffset = offset;
        remainingBufferSize -= bufferOffset;

        // Write headers to buffer
        std::size_t headerSize = 0;

        for (const auto& [name, value] : response.headers)
        {
            if (name.empty() || value.empty())
                continue;

            std::size_t requiredBufferSize = name.size() + 2 + value.size() + 2;

            if (requiredBufferSize > remainingBufferSize)
            {
                mg_http_reply(connection, 500, nullptr, "Internal Server Error");
                CLogger::LogPrint("HTTP server error: response max header size reached");
                return;
            }

            headerSize += requiredBufferSize;

            strncat_s(buffer.data() + bufferOffset, remainingBufferSize, name.c_str(), name.size());
            bufferOffset += name.size();
            remainingBufferSize -= name.size();

            strncat_s(buffer.data() + bufferOffset, remainingBufferSize, ": ", 2);
            bufferOffset += 2;
            remainingBufferSize -= 2;

            strncat_s(buffer.data() + bufferOffset, remainingBufferSize, value.c_str(), value.size());
            bufferOffset += value.size();
            remainingBufferSize -= value.size();

            strncat_s(buffer.data() + bufferOffset, remainingBufferSize, "\r\n", 2);
            bufferOffset += 2;
            remainingBufferSize -= 2;
        }

        if (headerSize > maxHeaderSize)
        {
            mg_http_reply(connection, 500, nullptr, "Internal Server Error");
            CLogger::LogPrint("HTTP server error: response max header size reached");
            return;
        }

        if (remainingBufferSize < 2)
        {
            mg_http_reply(connection, 500, nullptr, "Internal Server Error");
            CLogger::LogPrint("HTTP server error: response header not terminated");
            return;
        }

        strncat_s(buffer.data() + bufferOffset, remainingBufferSize, "\r\n", 2);
        bufferOffset += 2;
        remainingBufferSize -= 2;

        // Send response
        mg_send(connection, buffer.data(), bufferOffset);
        mg_send(connection, response.body.c_str(), response.body.size());
    }
}            // namespace mtasa
