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
    class HTTPServer;

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
