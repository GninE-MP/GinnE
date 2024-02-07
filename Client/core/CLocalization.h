/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CLocalization.h
 *  PURPOSE:     Automatically load required language and localize GninE text according to locale
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

using namespace tinygettext;

#include <core/CLocalizationInterface.h>
#include "CLanguage.h"
#define GninE_LOCALE_DIR  "GninE/locale/"

#pragma once

class CLocalization : public CLocalizationInterface
{
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
    SString              GetLanguageDirectory(CLanguage* pLanguage = nullptr);
    SString              GetLanguageCode();
    SString              GetLanguageName();
    SString              ValidateLocale(SString strLocale);
    void                 SetCurrentLanguage(SString strLocale = "");
    CLanguage*           GetLanguage(SString strLocale = "");
    SString              GetLanguageNativeName(SString strLocale = "");

    static void LogCallback(const std::string& str);

private:
    DictionaryManager             m_DictManager;
    std::map<SString, CLanguage*> m_LanguageMap;
    CLanguage*                    m_pCurrentLang;
};
