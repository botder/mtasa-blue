/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Win32 library enhancements
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>

namespace SharedUtil
{
    // Output a UTF8 encoded messagebox
    int MessageBoxUTF8(void* window, const std::string& text, const std::string& caption, unsigned int flags);
}            // namespace SharedUtil
