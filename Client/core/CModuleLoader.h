/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Dynamic module loading
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <windows.h>
#include <string>

class CModuleLoader
{
public:
    CModuleLoader(const std::string& ModuleName);
    CModuleLoader();
    ~CModuleLoader();

    bool LoadModule(const std::string& ModuleName);
    void UnloadModule();

    bool           IsOk() const { return m_bStatus; }
    const SString& GetLastErrorMessage() const;

    PVOID GetFunctionPointer(const std::string& FunctionName);

private:
    HMODULE m_hLoadedModule;
    bool    m_bStatus;
    SString m_strLastError;
};
