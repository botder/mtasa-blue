/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/CClient.h
 *  PURPOSE:     Header file for Client class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <core/CClientBase.h>

void internal_OUTPUT_VTABLE_DEBUG(const char* szLocation, const char* szTag, intptr_t variable);
void internal_OUTPUT_VTABLE_DEBUG(const char* szLocation, const char* szTag, CVehicleSAInterface* variable);
void internal_OUTPUT_VTABLE_DEBUG(const char* szLocation, const char* szTag, CEntity* variable);
#define OUTPUT_VTABLE_DEBUG(tag,var) internal_OUTPUT_VTABLE_DEBUG(__FUNCTION__, tag, var)

class CClient : public CClientBase
{
public:
    int  ClientInitialize(const char* szArguments, CCoreInterface* pCore);
    void ClientShutdown();

    void PreFrameExecutionHandler();
    void PreHUDRenderExecutionHandler(bool bDidUnminimize, bool bDidRecreateRenderTargets);
    void PostFrameExecutionHandler();
    void IdleHandler();
    void RestreamModel(unsigned short usModel);

    bool WebsiteRequestResultHandler(const std::unordered_set<SString>& newPages);

    bool ProcessCommand(const char* szCommandLine);
    // bool        ProcessInput                    ( CInputMessage* pInputMessage );

    bool HandleException(CExceptionInformation* pExceptionInformation);
    void GetPlayerNames(std::vector<SString>& vPlayerNames);
};
