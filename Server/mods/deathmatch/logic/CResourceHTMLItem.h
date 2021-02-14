/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource server-side HTML item class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CResourceFile.h"

// This class represents a single HTML resource item.
// This parses it and converts into into a script
class CResourceHTMLItem : public CResourceFile
{
public:
    CResourceHTMLItem(class CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes, bool bIsDefault,
                      bool bIsRaw, bool bRestricted, bool bOOPEnabled);
    ~CResourceHTMLItem();

    bool         Start();
    bool         Stop();

    bool ProcessRequest(mtasa::HTTPRequest& request, mtasa::HTTPResponse& response) override;

    bool AppendToPageBuffer(const char* szText, size_t length = 0);

    void SetResponseHeader(const char* szHeaderName, const char* szHeaderValue);
    void SetResponseCode(int responseCode);
    void SetResponseCookie(const char* szCookieName, const char* szCookieValue);
    void ClearPageBuffer();

    bool IsDefaultPage() { return m_bDefault; }
    void SetDefaultPage(bool bDefault) { m_bDefault = bDefault; }
    bool IsRestricted() { return m_bRestricted; };

private:
    char ReadChar(FILE* pFile) { return (unsigned char)fgetc(pFile); }
    void GetMimeType(const char* szFilename);

    bool        m_bIsBeingRequested;            // crude mutex
    bool        m_bIsRaw;
    CLuaMain*   m_pVM;
    std::string m_strPageBuffer;            // contains what we're sending
    bool        m_bDefault;                 // is this the default page for this resource?
    std::string m_strMime;
    bool        m_bRestricted;

    bool m_bOOPEnabled;

    mtasa::HTTPResponse* m_httpResponse;

    /*
    ResponseCode  m_responseCode;
    HttpResponse* m_currentResponse;
    */
};
