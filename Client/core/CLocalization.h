/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Automatically load required language and localize MTA text according to locale
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CLanguage.h"
#include <core/CLocalizationInterface.h>

class CLocalization : public CLocalizationInterface
{
public:
    inline static const auto MTA_LOCALE_DIR = "MTA/locale/";

public:
    CLocalization(const SString& strLocale = "", const SString& strLocalePath = "");
    ~CLocalization();

    SString Translate(const SString& strMessage);
    SString TranslateWithContext(const SString& strContext, const SString& strMessage);
    SString TranslatePlural(const SString& strSingular, const SString& strPlural, int iNum);
    SString TranslatePluralWithContext(const SString& strContext, const SString& strSingular, const SString& strPlural, int iNum);

    SString              GetTranslators();
    std::vector<SString> GetAvailableLocales();
    bool                 IsLocalized();
    SString              GetLanguageDirectory();
    SString              GetLanguageCode();
    SString              GetLanguageName();
    SString              ValidateLocale(SString strLocale);
    void                 SetCurrentLanguage(SString strLocale = "");
    CLanguage*           GetLanguage(SString strLocale = "");
    SString              GetLanguageNativeName(SString strLocale = "");

    static void LogCallback(const std::string& str);

private:
    tinygettext::DictionaryManager m_DictManager;
    std::map<SString, CLanguage*>  m_LanguageMap;
    CLanguage*                     m_pCurrentLang;
};

extern CLocalization* g_pLocalization;
