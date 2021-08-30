/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CHTTPD.h
 *  PURPOSE:     Built-in HTTP webserver class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <mtasa/IPBindableEndpoint.h>
#include <string_view>
#include <vector>
#include <memory>

class EHS;
class HttpRequest;
class HttpResponse;

class CHTTPD final
{
public:
    CHTTPD();
    ~CHTTPD();

public:
    bool Start(const std::vector<mtasa::IPBindableEndpoint>& bindings);
    void Stop();

    void SetDefaultResourceName(std::string_view resourceName);

    int RegisterEHS(EHS* ehs, const char* path);
    int UnregisterEHS(const char* path);

    int       RequestLogin(HttpRequest* request, HttpResponse* response);
    CAccount* CheckAuthentication(HttpRequest* request);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
    bool                  m_isRunning = false;
};
