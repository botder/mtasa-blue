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
#include <memory>

namespace mtasa
{
    class HTTPServer
    {
    public:
        void  SetHandle(void* handle) noexcept { m_handle = handle; }
        void* GetHandle() const noexcept { return m_handle; }

    private:
        void* m_handle;
    };

    class ServerFactory
    {
    public:
        static void Initialize();
        static void Shutdown();

        static std::unique_ptr<HTTPServer> CreateHTTPServer(const char* hostname, unsigned short port);
        static void                    DestroyServer(std::unique_ptr<HTTPServer>& server);

    private:
        static std::atomic_bool ms_isRunning;
        static std::thread      ms_worker;
    };
}            // namespace mtasa

/*
#include "ehs/ehs.h"

class CHTTPD : public EHS
{
public:
    CHTTPD();            // start the initial server
    ~CHTTPD();
    // EHS interface
    HttpResponse* RouteRequest(HttpRequest* ipoHttpRequest);
    ResponseCode  HandleRequest(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse);
    void          HttpPulse();
    bool          ShouldAllowConnection(const char* szAddress);

    // CHTTPD methods
    bool            StartHTTPD(const char* szIP, unsigned int port);
    bool            StopHTTPD();
    void            SetResource(CResource* resource) { m_resource = resource; }
    CResource*      GetResource() { return m_resource; }
    class CAccount* CheckAuthentication(HttpRequest* ipoHttpRequest);
    void            SetDefaultResource(const char* szResourceName) { m_strDefaultResourceName = szResourceName ? szResourceName : ""; }
    ResponseCode    RequestLogin(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse);

private:
    CResource*  m_resource;
    CHTTPD*     m_server;
    std::string m_strDefaultResourceName;            // default resource name

    EHSServerParameters m_Parameters;

    bool m_bStartedServer;

    class CAccount*        m_pGuestAccount;
    map<string, long long> m_LoggedInMap;
    CConnectHistory        m_BruteForceProtect;
    CConnectHistory        m_HttpDosProtect;
    std::set<SString>      m_HttpDosExcludeMap;
    std::mutex             m_mutexHttpDosProtect;
    std::mutex             m_mutexLoggedInMap;
    SString                m_strWarnMessageForIp;
    CElapsedTime           m_WarnMessageTimer;
};
*/
