/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Resource client-side script item class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CResourceFile.h"

// This class represents a single resource script item, being a .lua file
// It's task is to load and unload the script client side
class CResourceClientScriptItem : public CResourceFile
{
public:
    CResourceClientScriptItem(class CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes);
    ~CResourceClientScriptItem();

    bool Start();
    bool Stop();

    bool           IsNoClientCache() const { return m_bIsNoClientCache; }
    const SString& GetSourceCode() const { return m_sourceCode; }

    /*
    ResponseCode Request(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse);
    */

private:
    bool    m_bIsNoClientCache;
    SString m_sourceCode;
};
