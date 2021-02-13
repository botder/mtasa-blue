/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Built-in HTTP webserver
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <thread>

namespace mtasa
{
    class Webserver
    {
    public:
        static void Initialize();
        static void Shutdown();

    public:
        bool IsRunning() const noexcept { return m_isRunning; }

        void SetHostname(std::string hostname) noexcept { m_hostname = std::move(hostname); }
        void SetPort(unsigned short port) noexcept { m_port = port; }

        bool Start() noexcept;
        void Stop() noexcept;

    private:
        void WorkerThread();

    private:
        bool           m_isRunning = false;
        std::string    m_hostname;
        unsigned short m_port;

        void*       m_handle = nullptr;
        std::thread m_worker;
    };

    extern std::unique_ptr<Webserver> g_Webserver;
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
