/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Class to abstract a translation file to translated strings
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <tinygettext.hpp>

class CLanguage
{
public:
    CLanguage(const tinygettext::Dictionary& Dict, const SString& strLocale = "", const SString& strLangName = "");
    CLanguage(const SString& strPOPath);
    ~CLanguage();

    SString Translate(const SString& strMessage);
    SString TranslateWithContext(const SString& strContext, const SString& strMessage);
    SString TranslatePlural(const SString& strSingular, const SString& strPlural, int iNum);
    SString TranslatePluralWithContext(const SString& strContext, const SString& strSingular, const SString& strPlural, int iNum);

    SString     GetCode() { return m_strCode; }
    SString     GetName() { return m_strName; }
    tinygettext::Dictionary& GetDictionary() { return m_Dict; }

private:
    tinygettext::Dictionary m_Dict;
    SString                 m_strCode;            // Language code
    SString                 m_strName;            // Human readable name
};
