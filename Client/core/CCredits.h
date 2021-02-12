/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     In-game credits window
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <ctime>

class CGUIElement;
class CGUIWindow;
class CGUILabel;
class CGUIButton;

class CCredits
{
public:
    CCredits();
    ~CCredits();

    void SetVisible(bool bVisible);
    bool IsVisible();

    void Update();

    bool OnOKButtonClick(CGUIElement* pElement);

private:
    CGUIWindow* m_pWindow;
    CGUILabel*  m_pLabels[30];
    CGUIButton* m_pButtonOK;
    SString     m_strCredits;

    clock_t m_clkStart;
};
