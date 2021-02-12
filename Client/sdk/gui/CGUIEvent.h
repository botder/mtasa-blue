/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Event interface
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUITypes.h"
#include <CVector2D.h>

class CGUIElement;

struct CGUIEventArgs
{
    CGUIElement* pWindow;
};

struct CGUIKeyEventArgs
{
    CGUIElement* pWindow;

    unsigned int   codepoint;
    CGUIKeys::Scan scancode;
    unsigned int   sysKeys;
};

struct CGUIMouseEventArgs
{
    CGUIElement* pWindow;

    CGUIPosition           position;
    CVector2D              moveDelta;
    CGUIMouse::MouseButton button;
    unsigned int           sysKeys;
    float                  wheelChange;
    unsigned int           clickCount;
    CGUIElement*           pSwitchedWindow;
};

struct CGUIFocusEventArgs
{
    CGUIElement* pActivatedWindow;
    CGUIElement* pDeactivatedWindow;
};
