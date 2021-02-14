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
                mg_mgr_poll(&mongooseManager, 1000);
                mg_usleep(50'000);
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

        std::stringstream addressBuilder;
        addressBuilder << "http://" << hostname << ":" << port;

        std::string address = addressBuilder.str();

        auto server = std::make_unique<HTTPServer>();
        
        mg_connection* connection = mg_http_listen(&mongooseManager, address.c_str(), &MongooseHTTPServerCallback, server.get());
        server->SetHandle(connection);
        server->SetBaseAddress(std::move(address));

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
        }

        HTTPResponse response;
        server->ProcessRequest(request, response);

        if (!HTTPResponse::IsStandardStatusCode(response.statusCode))
        {
            mg_http_reply(connection, 500, nullptr, "Internal Server Error");
            CLogger::LogPrintf("HTTP server error: non-standard response status code %d", response.statusCode);
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

/*
CHTTPD::CHTTPD()
    : m_BruteForceProtect(4, 30000, 60000 * 5)            // Max of 4 attempts per 30 seconds, then 5 minute ignore
      ,
      m_HttpDosProtect(0, 0, 0)
{
    m_resource = NULL;
    m_server = NULL;
    m_bStartedServer = false;

    m_pGuestAccount = g_pGame->GetAccountManager()->AddGuestAccount(HTTP_GUEST_ACCOUNT_NAME);

    m_HttpDosProtect = CConnectHistory(g_pGame->GetConfig()->GetHTTPDosThreshold(), 10000,
                                       60000 * 1);            // Max of 'n' connections per 10 seconds, then 1 minute ignore

    std::vector<SString> excludeList;
    g_pGame->GetConfig()->GetHTTPDosExclude().Split(",", excludeList);
    m_HttpDosExcludeMap = std::set<SString>(excludeList.begin(), excludeList.end());
}
*/

/*
// Called from worker thread. g_pGame->Lock() has already been called.
// creates a page based on user input -- either displays data from
//   form or presents a form for users to submit data.
ResponseCode CHTTPD::HandleRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse)
{
    CAccount* account = CheckAuthentication(ipoHttpRequest);

    if (account)
    {
        if (!m_strDefaultResourceName.empty())
        {
            SString strWelcome("<a href='/%s/'>This is the page you want</a>", m_strDefaultResourceName.c_str());
            ipoHttpResponse->SetBody(strWelcome.c_str(), strWelcome.size());
            SString strNewURL("http://%s/%s/", ipoHttpRequest->oRequestHeaders["host"].c_str(), m_strDefaultResourceName.c_str());
            ipoHttpResponse->oResponseHeaders["location"] = strNewURL.c_str();
            return HTTPRESPONSECODE_302_FOUND;
        }
    }

    SString strWelcome(
        "You haven't set a default resource in your configuration file. You can either do this or visit http://%s/<i>resourcename</i>/ to see a specific "
        "resource.<br/><br/>Alternatively, the server may be still starting up, if so, please try again in a minute.",
        ipoHttpRequest->oRequestHeaders["host"].c_str());
    ipoHttpResponse->SetBody(strWelcome.c_str(), strWelcome.size());
    return HTTPRESPONSECODE_200_OK;
}

ResponseCode CHTTPD::RequestLogin(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse)
{
    if (m_WarnMessageTimer.Get() < 4000 && m_strWarnMessageForIp == ipoHttpRequest->GetAddress())
    {
        SString strMessage;
        strMessage += SString("Your IP address ('%s') is not associated with an authorized serial.", ipoHttpRequest->GetAddress().c_str());
        strMessage += SString("<br/><a href='%s'>See here for more information</a>",
                              "https:"
                              "//mtasa.com/authserialhttp");
        ipoHttpResponse->SetBody(strMessage, strMessage.length());
        return HTTPRESPONSECODE_401_UNAUTHORIZED;
    }

    const char* szAuthenticateMessage = "Access denied, please login";
    ipoHttpResponse->SetBody(szAuthenticateMessage, strlen(szAuthenticateMessage));
    SString strName("Basic realm=\"%s\"", g_pGame->GetConfig()->GetServerName().c_str());
    ipoHttpResponse->oResponseHeaders["WWW-Authenticate"] = strName.c_str();
    return HTTPRESPONSECODE_401_UNAUTHORIZED;
}

CAccount* CHTTPD::CheckAuthentication(HttpRequest* ipoHttpRequest)
{
    string authorization = ipoHttpRequest->oRequestHeaders["authorization"];
    if (authorization.length() > 6)
    {
        if (authorization.substr(0, 6) == "Basic ")
        {
            // Basic auth
            SString strAuthDecoded = SharedUtil::Base64decode(authorization.substr(6));

            SString authName, authPassword;
            strAuthDecoded.Split(":", &authName, &authPassword);

            if (m_BruteForceProtect.IsFlooding(ipoHttpRequest->GetAddress().c_str()))
            {
                CLogger::AuthPrintf("HTTP: Ignoring login attempt for user '%s' from %s\n", authName.c_str(), ipoHttpRequest->GetAddress().c_str());
                return m_pGuestAccount;
            }

            CAccount* account = g_pGame->GetAccountManager()->Get(authName.c_str());
            if (account)
            {
                // Check that the password is right
                bool bSkipIpCheck;
                if (account->IsPassword(authPassword.c_str(), &bSkipIpCheck))
                {
                    // Check that it isn't the Console account
                    std::string strAccountName = account->GetName();
                    if (strAccountName.compare(CONSOLE_ACCOUNT_NAME) != 0)
                    {
                        // Do IP check if required
                        if (!bSkipIpCheck && !g_pGame->GetAccountManager()->IsHttpLoginAllowed(account, ipoHttpRequest->GetAddress()))
                        {
                            if (m_WarnMessageTimer.Get() > 8000 || m_strWarnMessageForIp != ipoHttpRequest->GetAddress())
                            {
                                m_strWarnMessageForIp = ipoHttpRequest->GetAddress();
                                m_WarnMessageTimer.Reset();
                            }
                            CLogger::AuthPrintf("HTTP: Failed login for user '%s' because %s not associated with authorized serial\n", authName.c_str(),
                                                ipoHttpRequest->GetAddress().c_str());
                            return m_pGuestAccount;
                        }

                        // Handle initial login logging
                        std::lock_guard<std::mutex> guard(m_mutexLoggedInMap);
                        if (m_LoggedInMap.find(authName) == m_LoggedInMap.end())
                            CLogger::AuthPrintf("HTTP: '%s' entered correct password from %s\n", authName.c_str(), ipoHttpRequest->GetAddress().c_str());
                        m_LoggedInMap[authName] = GetTickCount64_();
                        account->OnLoginHttpSuccess(ipoHttpRequest->GetAddress());
                        return account;
                    }
                }
            }
            if (authName.length() > 0)
            {
                m_BruteForceProtect.AddConnect(ipoHttpRequest->GetAddress().c_str());
                CLogger::AuthPrintf("HTTP: Failed login attempt for user '%s' from %s\n", authName.c_str(), ipoHttpRequest->GetAddress().c_str());
            }
        }
    }
    return m_pGuestAccount;
}

// Called from worker thread. Careful now.
void CHTTPD::HttpPulse()
{
    std::lock_guard<std::mutex> guard(m_mutexLoggedInMap);

    long long llExpireTime = GetTickCount64_() - 1000 * 60 * 5;            // 5 minute timeout

    map<string, long long>::iterator iter = m_LoggedInMap.begin();
    while (iter != m_LoggedInMap.end())
    {
        // Remove if too long since last request
        if (iter->second < llExpireTime)
        {
            CLogger::AuthPrintf("HTTP: '%s' no longer connected\n", iter->first.c_str());
            m_LoggedInMap.erase(iter++);
        }
        else
            iter++;
    }
}

//
// Do DoS check here
// Called from worker thread. Careful now.
bool CHTTPD::ShouldAllowConnection(const char* szAddress)
{
    std::lock_guard<std::mutex> guard(m_mutexHttpDosProtect);

    if (MapContains(m_HttpDosExcludeMap, szAddress))
        return true;

    if (m_HttpDosProtect.IsFlooding(szAddress))
        return false;

    m_HttpDosProtect.AddConnect(szAddress);

    if (m_HttpDosProtect.IsFlooding(szAddress))
    {
        CLogger::AuthPrintf("HTTP: Connection flood from '%s'. Ignoring for 1 min.\n", szAddress);
        return false;
    }

    return true;
}
*/