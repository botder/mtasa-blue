/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Class to abstract a translation file to translated strings
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLanguage.h"
#include "po_parser.hpp"
#include <fstream>

CLanguage::CLanguage(const tinygettext::Dictionary& Dict, const SString& strLocale, const SString& strLangName)
{
    m_Dict = Dict;
    m_strCode = strLocale;
    m_strName = strLangName;
}

CLanguage::CLanguage(const SString& strPOPath)
{
    std::ifstream in(FromUTF8(strPOPath));
    tinygettext::POParser::parse(strPOPath, in, m_Dict);
    in.close();
}

CLanguage::~CLanguage()
{
}

SString CLanguage::Translate(const SString& strMessage)
{
    return SString(m_Dict.translate(strMessage));
}

SString CLanguage::TranslateWithContext(const SString& strContext, const SString& strMessage)
{
    return SString(m_Dict.translate_ctxt(strContext, strMessage));
}

SString CLanguage::TranslatePlural(const SString& strSingular, const SString& strPlural, int iNum)
{
    return SString(m_Dict.translate_plural(strSingular, strPlural, iNum));
}

SString CLanguage::TranslatePluralWithContext(const SString& strContext, const SString& strSingular, const SString& strPlural, int iNum)
{
    return SString(m_Dict.translate_ctxt_plural(strContext, strSingular, strPlural, iNum));
}
