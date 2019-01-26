/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/Common.h
 *  PURPOSE:     Common game layer include file
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define CGAME_DLL

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <game/Common.h>

class CVehicleSAInterface;
class CEntitySAInterface;
void internal_OUTPUT_VTABLE_DEBUG(const char* szLocation, const char* szTag, intptr_t variable);
void internal_OUTPUT_VTABLE_DEBUG(const char* szLocation, const char* szTag, CVehicleSAInterface* variable);
void internal_OUTPUT_VTABLE_DEBUG(const char* szLocation, const char* szTag, CEntitySAInterface* variable);
#define OUTPUT_VTABLE_DEBUG(tag,var) internal_OUTPUT_VTABLE_DEBUG(__FUNCTION__, tag, var)

#undef DEBUG_LOG
#ifdef DEBUG_LOG
    #include <stdio.h>
    #include <time.h>

static FILE* fDebugFile;

static void OutputDebugText(char* szDebugText)
{
        #ifdef MTA_DEBUG
    char szDebug[500];
    sprintf(szDebug, "%s\n", szDebugText);
    OutputDebugString(szDebug);
        #endif
}

    #ifdef MTA_DEBUG
        #define DEBUG_TRACE(szText) \
            OutputDebugString(szText);
    #endif
#else
    #define DEBUG_TRACE(szText) // we do nothing with release versions
#endif
