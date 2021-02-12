/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <vector>

class CGUIWindow;
class CGUIElement;
class CGUITab;
class CGUITabPanel;
class CGUIButton;

struct SNewsItem
{
    SString              strContentFullDir;
    SString              strHeadline;
    SString              strDate;
    SString              strLayoutFilename;
    std::vector<SString> imagesetFilenameList;
};

class CNewsBrowser
{
public:
    ZERO_ON_NEW
    CNewsBrowser();
    ~CNewsBrowser();

    void SetVisible(bool bVisible);
    bool IsVisible();
    void CreateHeadlines();
    void SwitchToTab(int iIndex);

protected:
    void        InitNewsItemList();
    void        AddNewsTab(const SNewsItem& newsItem);
    CGUIWindow* LoadLayoutAndImages(CGUIElement* pParent, const SNewsItem& newsItem);
    void        CreateGUI();
    void        DestroyGUI();
    bool        OnOKButtonClick(CGUIElement* pElement);
    bool        OnNewsLinkButtonClick(CGUIElement* pElement);

    std::vector<SNewsItem>   m_NewsitemList;
    CGUIWindow*              m_pWindow;
    CGUITabPanel*            m_pTabPanel;
    CGUIButton*              m_pButtonOK;
    CGUIButton*              m_pButtonNewsLink;
    std::vector<CGUITab*>    m_TabList;
    std::vector<CGUIWindow*> m_TabContentList;
};
