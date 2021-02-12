/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Console Logging functionality
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CSingleton.h"
#include <fstream>
#include <string>

class CConsoleLogger : public CSingleton<CConsoleLogger>
{
public:
    CConsoleLogger();
    ~CConsoleLogger();

    void LinePrintf(const char* szFormat, ...);
    void WriteLine(const std::string& strLine);

private:
    std::string  m_strFilename;
    FILE*        m_pFile;
    std::fstream File;
};
