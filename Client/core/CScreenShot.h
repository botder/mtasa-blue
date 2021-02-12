/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Screen capture file handling
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <time.h>

class CScreenShot
{
public:
    static SString PreScreenShot();
    static void    PostScreenShot(const SString& strFileName);
    static void    SetPath(const char* szPath);

    static SString GetScreenShotPath(int iNumber);
    static SString GetValidScreenshotFilename();
    static int     GetScreenShots();

    static void          BeginSave(const char* szFileName, void* pData, unsigned int uiDataSize, unsigned int uiWidth, unsigned int uiHeight);
    static bool          IsSaving();
    static unsigned long ThreadProc(void* lpdwThreadParam);
};
