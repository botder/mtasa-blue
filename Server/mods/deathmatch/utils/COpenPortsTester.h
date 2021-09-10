/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/utils/COpenPortsTester.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "ASE.h"

class COpenPortsTester final
{
    struct TestContext
    {
        std::string            description;
        mtasa::IPAddressFamily connectionType;
        bool                   isEnabled = true;
        bool                   isRunning = false;
        std::uint16_t          gamePort = 0;
        std::uint16_t          queryPort = 0;
        std::uint16_t          httpPort = 0;

        TestContext(const char* description_, mtasa::IPAddressFamily connectionType_) : description{description_}, connectionType{connectionType_} {}
    };

public:
    COpenPortsTester(mtasa::IPAddressFamily connectionType)
    {
        switch (connectionType)
        {
            case mtasa::IPAddressFamily::Unspecified:
                break;
            case mtasa::IPAddressFamily::IPv4:
                m_ipv6.isEnabled = false;
                break;
            case mtasa::IPAddressFamily::IPv6:
                m_ipv4.isEnabled = false;
                break;
        }
    }

    bool IsRunning() const noexcept { return m_ipv4.isRunning || m_ipv6.isRunning; }

    void Start()
    {
        if (IsRunning())
            return;

        std::uint16_t serverPort = g_pGame->GetConfig()->GetServerPort();
        std::uint16_t queryPort = serverPort + SERVER_LIST_QUERY_PORT_OFFSET;

        std::stringstream stream;
        stream << PORT_TESTER_URL "?simple=1&g=" << serverPort;

        if (ASE::GetInstance())
        {
            stream << "&a=" << queryPort;
        }
        else
        {
            CLogger::LogPrintfNoStamp("ASE is not enabled, so port UDP port %u will not be tested\n", queryPort);
        }

        std::uint16_t httpPort = g_pGame->GetConfig()->GetHTTPPort();

        if (g_pGame->GetHTTPD())
        {
            stream << "&h=" << httpPort;
        }
        else
        {
            CLogger::LogPrintfNoStamp("HTTP server is not enabled, so port TCP port %u will not be tested\n", httpPort);
        }

        std::string url = stream.str();

#if MTA_DEBUG
        OutputDebugLine(SString("Testing open ports with url: %s", url.c_str()));
#endif

        m_ipv4.gamePort = serverPort;
        m_ipv4.queryPort = queryPort;
        m_ipv4.httpPort = httpPort;
        QueueTest(url.c_str(), m_ipv4);

        m_ipv6.gamePort = serverPort;
        m_ipv6.queryPort = queryPort;
        m_ipv6.httpPort = httpPort;
        QueueTest(url.c_str(), m_ipv6);

        if (IsRunning())
            CLogger::LogPrintfNoStamp("Testing ports...\n");
    }

    //
    // Process response from remote
    //
    static void DownloadFinishedCallback(const SHttpDownloadResult& result)
    {
        auto& context = *reinterpret_cast<TestContext*>(result.pObj);
        context.isRunning = false;

        if (!result.bSuccess || result.iErrorCode != 200)
        {
            CLogger::LogPrintfNoStamp("[%s] Port testing service unavailable! (%u %s)\n", context.description.c_str(), result.iErrorCode,
                                      GetDownloadManager()->GetError());
            return;
        }

        // Parse the response
        std::string_view response{result.pData, result.dataSize};
        std::size_t      start = response.find_first_not_of("&\n");
        std::size_t      stop = response.find_last_not_of("&\n");

        if (start == std::string_view::npos || stop == std::string_view::npos)
        {
            CLogger::LogPrintfNoStamp("[%s] Unexpected response in port test:\n%.*s\n", context.description.c_str(), response.size(), response.data());
            return;
        }

        std::string_view parameters = response.substr(start, stop - start + 1);
        std::size_t      numOutputLines = 0;

        while (!parameters.empty())
        {
            std::size_t      delimiterPos = parameters.find_first_of('&');
            std::string_view param;

            if (delimiterPos != std::string_view::npos)
            {
                param = parameters.substr(0, delimiterPos);
                parameters = parameters.substr(delimiterPos + 1);
            }
            else
            {
                param = std::exchange(parameters, {});
            }

            // "k=v" (k is key, v is value)
            if (param.size() != 3 || param[1] != '=')
                continue;

            std::string_view action{"?"};
            std::string_view protocol{"UDP"};
            std::uint16_t    port = 0;

            switch (param[0])
            {
                case 'a':
                    action = "browse";
                    port = context.queryPort;
                    break;
                case 'g':
                    action = "join";
                    port = context.gamePort;
                    break;
                case 'h':
                    action = "download";
                    port = context.httpPort;
                    break;
                default:
                    break;
            }

            if (param[1] == '1')
            {
                CLogger::LogPrintfNoStamp("[%s] Port %u %.*s is open.\n", context.description.c_str(), port, protocol.size(), protocol.data());
            }
            else
            {
                CLogger::LogPrintfNoStamp("[%s] Port %u %.*s is closed. Players can not %.*s!\n", context.description.c_str(), port, protocol.size(),
                                          protocol.data(), action.size(), action.data());
            }

            ++numOutputLines;
        }

        if (!numOutputLines)
            CLogger::LogPrintfNoStamp("[%s] Unexpected response in port test:\n%.*s\n", context.description.c_str(), response.size(), response.data());
    }

    //
    // Get http downloader used for port testing
    //
    static CNetHTTPDownloadManagerInterface* GetDownloadManager() { return g_pNetServer->GetHTTPDownloadManager(EDownloadMode::ASE); }

private:
    void QueueTest(const char* url, TestContext& context)
    {
        if (!context.isEnabled)
            return;

#if MTA_DEBUG
        OutputDebugLine(SString("Running port test for: %s", context.description.c_str()));
#endif

        SHttpRequestOptions options;
        options.uiConnectionAttempts = 1;
        options.uiConnectTimeoutMs = 15000;
        options.connectionType = context.connectionType;
        context.isRunning = GetDownloadManager()->QueueFile(url, nullptr, &context, DownloadFinishedCallback, options);
    }

private:
    TestContext m_ipv4{"IPv4", mtasa::IPAddressFamily::IPv4};
    TestContext m_ipv6{"IPv6", mtasa::IPAddressFamily::IPv6};
};
