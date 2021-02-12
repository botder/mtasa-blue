/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Win32 library enhancements
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "SharedUtil.Win32.h"
#include "SharedUtil.Misc.h"
#include <windows.h>

namespace SharedUtil
{
    int MessageBoxUTF8(void* window, const std::string& text, const std::string& caption, unsigned int flags)
    {
        // Default to warning icon
        if ((flags & (MB_ICONERROR | MB_ICONQUESTION | MB_ICONWARNING | MB_ICONINFORMATION)) == 0)
            flags |= MB_ICONWARNING;

        // Make topmost work
        if (flags & MB_TOPMOST)
            flags |= MB_SYSTEMMODAL;

        WString strText = MbUTF8ToUTF16(text);
        WString strCaption = MbUTF8ToUTF16(caption);
        return MessageBoxW(reinterpret_cast<HWND>(window), strText.c_str(), strCaption.c_str(), flags);
    }
}            // namespace SharedUtil
