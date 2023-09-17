//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : IHunSpell.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Name:        IHunSpell.cpp
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     02/02/14
// SVN-ID:      $Id: IHunSpell.cpp 35 2014-02-22 18:18:49Z Frank $
// Copyright:   2014 Frank Lichtner
// License:
/////////////////////////////////////////////////////////////////////////////
// For compilers that support precompilation, includes "wx/wx.h".
#include "file_logger.h"
#include "globals.h"
#include "macros.h"
#include "wx/wxprec.h"

#include <wx/log.h>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "CorrectSpellingDlg.h"
#include "IHunSpell.h"
#include "ctags_manager.h"
#include "scGlobals.h"
#include "spellcheck.h"

#include <wx/arrimpl.cpp>
#include <wx/filename.h>
#include <wx/regex.h>
#include <wx/stc/stc.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>

namespace
{
/// Keep a set of styles per lexer-id that should be checked for spelling

/// Only STRING styles
std::unordered_map<int, std::unordered_set<int>> ALLOWED_STYLES_STRINGS = {
    { wxSTC_LEX_ASM, { wxSTC_ASM_STRING } },
    { wxSTC_LEX_BASH, { wxSTC_SH_STRING } },
    { wxSTC_LEX_CMAKE, { wxSTC_CMAKE_STRINGDQ, wxSTC_CMAKE_STRINGLQ, wxSTC_CMAKE_STRINGRQ } },
    { wxSTC_LEX_CSS, { wxSTC_CSS_DOUBLESTRING, wxSTC_CSS_SINGLESTRING } },
    { wxSTC_LEX_CPP, { wxSTC_C_STRING, wxSTC_C_STRINGRAW, wxSTC_C_STRINGEOL } },
    { wxSTC_LEX_FORTRAN, { wxSTC_F_STRING1, wxSTC_F_STRING2 } },
    { wxSTC_LEX_INNOSETUP, { wxSTC_INNO_STRING_DOUBLE, wxSTC_INNO_STRING_SINGLE } },
    { wxSTC_LEX_JSON, { wxSTC_JSON_STRING, wxSTC_JSON_STRINGEOL } },
    { wxSTC_LEX_LUA, { wxSTC_LUA_STRING, wxSTC_LUA_LITERALSTRING, wxSTC_LUA_STRINGEOL } },
    { wxSTC_LEX_MARKDOWN, { wxSTC_MARKDOWN_DEFAULT } },
    { wxSTC_LEX_HTML,
      {
          wxSTC_H_DOUBLESTRING,
          wxSTC_H_SINGLESTRING,
          wxSTC_HJ_DOUBLESTRING,
          wxSTC_HJ_SINGLESTRING,
          wxSTC_HJ_STRINGEOL,
          wxSTC_HPHP_HSTRING,
          wxSTC_HPHP_SIMPLESTRING,
      } },
    { wxSTC_LEX_PYTHON,
      {
          wxSTC_P_STRING,
          wxSTC_P_STRINGEOL,
          wxSTC_P_TRIPLE,
          wxSTC_P_TRIPLEDOUBLE,
#if wxCHECK_VERSION(3, 3, 0)
          wxSTC_P_FSTRING,
          wxSTC_P_FCHARACTER,
          wxSTC_P_FTRIPLE,
          wxSTC_P_FTRIPLEDOUBLE,
#endif
      } },
    { wxSTC_LEX_RUBY,
      {
          wxSTC_RB_STRING,
          wxSTC_RB_STRING_Q,
          wxSTC_RB_STRING_QQ,
          wxSTC_RB_STRING_QX,
          wxSTC_RB_STRING_QR,
          wxSTC_RB_STRING_QW,
      } },
    { wxSTC_LEX_RUST,
      {
          wxSTC_RUST_STRING,
          wxSTC_RUST_STRINGR,
          wxSTC_RUST_BYTESTRING,
          wxSTC_RUST_BYTESTRINGR,
      } },
    { wxSTC_LEX_NULL, { 0 } },
};

/// Only COMMENT styles
std::unordered_map<int, std::unordered_set<int>> ALLOWED_STYLES_COMMENTS = {
    { wxSTC_LEX_ASM, { wxSTC_ASM_COMMENT, wxSTC_ASM_COMMENTBLOCK, wxSTC_ASM_COMMENTDIRECTIVE } },
    { wxSTC_LEX_BASH, { wxSTC_SH_COMMENTLINE } },
    { wxSTC_LEX_BATCH, { wxSTC_BAT_COMMENT } },
    { wxSTC_LEX_CMAKE, { wxSTC_CMAKE_COMMENT } },
    { wxSTC_LEX_CSS, { wxSTC_CSS_COMMENT } },
    { wxSTC_LEX_CPP, { wxSTC_C_COMMENT, wxSTC_C_COMMENTDOC, wxSTC_C_COMMENTLINE, wxSTC_C_COMMENTLINEDOC } },
    { wxSTC_LEX_DIFF, { wxSTC_DIFF_COMMENT } },
    { wxSTC_LEX_FORTRAN, { wxSTC_F_COMMENT } },
    { wxSTC_LEX_PROPERTIES, { wxSTC_PROPS_COMMENT } },
    { wxSTC_LEX_INNOSETUP, { wxSTC_INNO_COMMENT } },
    { wxSTC_LEX_JSON, { wxSTC_JSON_LINECOMMENT, wxSTC_JSON_BLOCKCOMMENT } },
    { wxSTC_LEX_LUA, { wxSTC_LUA_COMMENT, wxSTC_LUA_COMMENTLINE, wxSTC_LUA_COMMENTDOC } },
    { wxSTC_LEX_MAKEFILE, { wxSTC_MAKE_COMMENT } },
    { wxSTC_LEX_MARKDOWN, { wxSTC_MARKDOWN_DEFAULT } },
    { wxSTC_LEX_HTML,
      {
          wxSTC_H_COMMENT,
          wxSTC_HJ_COMMENT,
          wxSTC_HJ_COMMENTLINE,
          wxSTC_HJ_COMMENTDOC,
          wxSTC_HPHP_COMMENTLINE,
          wxSTC_HPHP_COMMENT,
      } },
    { wxSTC_LEX_PYTHON, { wxSTC_P_COMMENTLINE, wxSTC_P_COMMENTBLOCK } },
    { wxSTC_LEX_RUBY, { wxSTC_RB_COMMENTLINE } },
    { wxSTC_LEX_RUST,
      {
          wxSTC_RUST_COMMENTBLOCK,
          wxSTC_RUST_COMMENTLINE,
          wxSTC_RUST_COMMENTBLOCKDOC,
          wxSTC_RUST_COMMENTLINEDOC,
      } },
    { wxSTC_LEX_SQL, { wxSTC_SQL_COMMENT, wxSTC_SQL_COMMENTLINE, wxSTC_SQL_COMMENTDOC } },
    { wxSTC_LEX_TCL, { wxSTC_TCL_COMMENT, wxSTC_TCL_COMMENTLINE, wxSTC_TCL_BLOCK_COMMENT, wxSTC_TCL_COMMENT_BOX } },
    { wxSTC_LEX_NULL, { 0 } },
    { wxSTC_LEX_XML, { wxSTC_H_COMMENT } },
    { wxSTC_LEX_YAML, { wxSTC_YAML_COMMENT } },
};
} // namespace

// ------------------------------------------------------------
#define MIN_TOKEN_LEN 3
// ------------------------------------------------------------
IHunSpell::IHunSpell()
    : m_caseSensitiveUserDictionary(true)
    , m_ignoreSymbolsInTagsDatabase(false)
    , m_pSpell(nullptr)
    , m_pPlugIn(nullptr)
    , m_pSpellDlg(nullptr)
    , m_scanners(0)
{
    InitLanguageList();
}
// ------------------------------------------------------------
IHunSpell::~IHunSpell()
{
    CloseEngine();

    if(m_pSpellDlg != NULL)
        m_pSpellDlg->Destroy();
}
// ------------------------------------------------------------
bool IHunSpell::InitEngine()
{
    // check if we are already initialized
    if(m_pSpell != NULL)
        return true;

    m_ignoreList = CustomDictionary(0, StringHashOptionalCase(m_caseSensitiveUserDictionary),
                                    StringCompareOptionalCase(m_caseSensitiveUserDictionary));
    m_userDict = CustomDictionary(0, StringHashOptionalCase(m_caseSensitiveUserDictionary),
                                  StringCompareOptionalCase(m_caseSensitiveUserDictionary));

    // check base path
    if(!m_dicPath.IsEmpty() && !wxEndsWithPathSeparator(m_dicPath))
        m_dicPath += wxFILE_SEP_PATH;
    // load user dict
    LoadUserDict(m_userDictPath + s_userDict);
    // create dictionary path
    wxString dict = m_dicPath + m_dictionary + wxT(".aff");
    wxString aff = m_dicPath + m_dictionary + wxT(".dic");

    wxCharBuffer affBuffer = dict.mb_str();
    wxCharBuffer dicBuffer = aff.mb_str();

    wxFileName fna(aff);

    if(!fna.FileExists()) {
        clLogMessage(_("Could not find aff file!"));
        return false;
    }
    wxFileName fnd(dict);

    if(!fnd.FileExists()) {
        clLogMessage(_("Could not find dictionary file!"));
        return false;
    }
    // so far ok, init engine
    m_pSpell = Hunspell_create(affBuffer, dicBuffer);
    return true;
}

// ------------------------------------------------------------
void IHunSpell::CloseEngine()
{
    if(m_pSpell != NULL) {
        Hunspell_destroy(m_pSpell);
        SaveUserDict(m_userDictPath + s_userDict);
    }
    m_pSpell = NULL;
}
// ------------------------------------------------------------
bool IHunSpell::CheckWord(const wxString& word) const
{
    static thread_local wxRegEx rehex(s_dectHex, wxRE_ADVANCED);

    // look in ignore list
    if(m_ignoreList.count(word) != 0)
        return true;

    // look in user list
    if(m_userDict.count(word) != 0)
        return true;

    // see if hex number
    if(rehex.Matches(word))
        return true;

    return Hunspell_spell(m_pSpell, word.ToUTF8()) != 0;
}
// ------------------------------------------------------------
bool IHunSpell::IsTag(const wxString& word) const
{
    if(GetIgnoreSymbolsInTagsDatabase()) {
        std::vector<TagEntryPtr> tags;
        TagsManagerST::Get()->FindSymbol(word, tags);
        if(!tags.empty())
            return true;
    }

    return false;
}
// ------------------------------------------------------------
wxArrayString IHunSpell::GetSuggestions(const wxString& misspelled)
{
    wxArrayString suggestions;
    suggestions.Empty();

    if(m_pSpell) {
        char** wlst;

        wxCharBuffer misspelledBuffer = misspelled.mb_str();
        int ns = Hunspell_suggest(m_pSpell, &wlst, misspelledBuffer);

        for(int i = 0; i < ns; i++)
            suggestions.Add(wxString(wlst[i]));
        Hunspell_free_list(m_pSpell, &wlst, ns);
    }

    return suggestions;
}

// ------------------------------------------------------------
void IHunSpell::CheckSpelling()
{
    LOG_IF_TRACE { clDEBUG1() << "SpellChecker: checkSpelling is called" << endl; }
    IEditor* pEditor = ::clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(pEditor);
    CHECK_COND_RET(InitEngine());

    int offset = 0;
    bool error = false;
    wxString text = pEditor->GetEditorText() + " ";

    // check for dialog and create if necessary
    if(m_pSpellDlg == NULL) {
        m_pSpellDlg = new CorrectSpellingDlg(NULL);
    }
    m_pSpellDlg->SetPHs(this);
    wxStringTokenizer tkz(text, s_defDelimiters);

    const std::unordered_set<int>* STRING_STYLES = nullptr;
    const std::unordered_set<int>* COMMENT_STYLES = nullptr;

    if(ALLOWED_STYLES_STRINGS.count(pEditor->GetLexerId())) {
        STRING_STYLES = &ALLOWED_STYLES_STRINGS[pEditor->GetLexerId()];
    }

    if(ALLOWED_STYLES_COMMENTS.count(pEditor->GetLexerId())) {
        COMMENT_STYLES = &ALLOWED_STYLES_COMMENTS[pEditor->GetLexerId()];
    }

#define IS_STYLE_ALLOWED(pset, style_id) \
    (!pset /* no limit */ || (pset && pset->count(style_id) /* we have a set -> it must contain the id*/))

    LOG_IF_TRACE { clDEBUG1() << "SpellChecker: checking file:" << pEditor->GetFileName() << endl; }
    while(tkz.HasMoreTokens()) {
        wxString token = tkz.GetNextToken();
        int pos = tkz.GetPosition() - token.length() - 1;
        pos += offset;

        // ignore token shorter then MIN_TOKEN_LEN
        if(token.length() <= MIN_TOKEN_LEN)
            continue;

        // Check the style at the middle of the token
        int style_at_pos = pEditor->GetStyleAtPos(pos + token.length() / 2);
        if(!IS_STYLE_ALLOWED(STRING_STYLES, style_at_pos) && !IS_STYLE_ALLOWED(COMMENT_STYLES, style_at_pos))
            // skip this token
            continue;

        // process token
        if(!CheckWord(token)) {
            pEditor->SetUserIndicator(pos, token.length());

            if(!m_pPlugIn->GetCheckContinuous()) {
                pEditor->SetCaretAt(pos);
                pEditor->SelectText(pos, token.length());
                // show correct spelling dialog
                error = true;
                m_pSpellDlg->SetMisspelled(token);
                m_pSpellDlg->SetSuggestions(GetSuggestions(token));
                int ret = m_pSpellDlg->ShowModal();

                switch(ret) {
                case SC_CHANGE: {
                    // correct spelling
                    wxString replace = m_pSpellDlg->GetMisspelled();
                    offset += replace.Len() - token.Len();
                    text.replace(pos, token.Len(), replace);
                    pEditor->ReplaceSelection(replace);
                } break;
                case SC_IGNORE:
                    AddWordToIgnoreList(token);
                    break;
                case SC_ADD:
                    AddWordToUserDict(token);
                    break;
                default: {
                    pEditor->ClearUserIndicators();
                    return;
                }
                }
            }
        }
    }
#undef IS_STYLE_ALLOWED

    LOG_IF_TRACE { clDEBUG1() << "SpellChecker:: checking file:" << pEditor->GetFileName() << "is done" << endl; }
    if(!m_pPlugIn->GetCheckContinuous()) {
        // clean up
        pEditor->ClearUserIndicators();

        if(!error)
            ::wxMessageBox(_("No spelling errors found!"));
    }
}
// ------------------------------------------------------------
// tools
// ------------------------------------------------------------
wxString IHunSpell::GetCharacterEncoding()
{
    if(m_pSpell == NULL)
        return wxEmptyString;

    wxString encoding(wxConvUTF8.cMB2WC(Hunspell_get_dic_encoding(m_pSpell)), *wxConvCurrent);
    return encoding;
}

// ------------------------------------------------------------
void IHunSpell::AddWordToIgnoreList(const wxString& word)
{
    if(word.IsEmpty())
        return;

    m_ignoreList.insert(word);
}
// ------------------------------------------------------------
void IHunSpell::AddWordToUserDict(const wxString& word)
{
    if(word.IsEmpty())
        return;

    m_userDict.insert(word);
}
// ------------------------------------------------------------
bool IHunSpell::LoadUserDict(const wxString& filename)
{
    wxTextFile tf(filename);

    if(!tf.Exists())
        return false;

    m_userDict.clear();

    tf.Open();

    for(wxUint32 i = 0; i < tf.GetLineCount(); i++) {
        m_userDict.insert(tf.GetLine(i));
    }
    tf.Close();

    return true;
}
// ------------------------------------------------------------
bool IHunSpell::SaveUserDict(const wxString& filename)
{
    wxTextFile tf(filename);
    CustomDictionary fileUserDict(m_userDict);

    if(!tf.Exists()) {
        if(!tf.Create())
            return false;
    } else {
        if(!tf.Open())
            return false;

        // Re-read contents from disk in case another CodeLite instance has updated the user dictionary.
        for(wxUint32 i = 0; i < tf.GetLineCount(); i++) {
            fileUserDict.insert(tf.GetLine(i));
        }

        tf.Clear();
    }

    for(const auto& word : fileUserDict) {
        tf.AddLine(word);
    }
    tf.Write();
    tf.Close();

    return true;
}
// ------------------------------------------------------------
bool IHunSpell::ChangeLanguage(const wxString& language)
{
    if(m_dictionary.Cmp(language) == 0)
        return false;
    CloseEngine();
    m_dictionary = language;
    return InitEngine();
}
// ------------------------------------------------------------
void IHunSpell::InitLanguageList()
{
    m_languageList[_T( "Afrikaans (South Africa)" )] = _T( "af_ZA" );
    m_languageList[_T( "Bulgarian (Bulgaria)" )] = _T( "bg_BG" );
    m_languageList[_T( "Catalan (Spain)" )] = _T( "ca_ES" );
    m_languageList[_T( "Czech (Czech Republic)" )] = _T( "cs_CZ" );
    m_languageList[_T( "Welsh (Wales)" )] = _T( "cy_GB" );
    m_languageList[_T( "Danish (Denmark)" )] = _T( "da_DK" );
    m_languageList[_T( "German (Austria)" )] = _T( "de_AT" );
    m_languageList[_T( "German (Switzerland)" )] = _T( "de_CH" );
    m_languageList[_T( "German (Germany- orig dict)" )] = _T( "de_DE" );
    m_languageList[_T( "German (Germany-old & neu ortho.)" )] = _T( "de_DE_comb" );
    m_languageList[_T( "German (Germany-neu ortho.)" )] = _T( "de_DE_neu" );
    m_languageList[_T( "Greek (Greece)" )] = _T( "el_GR" );
    m_languageList[_T( "English (Australia)" )] = _T( "en_AU" );
    m_languageList[_T( "English (Canada)" )] = _T("en_CA");
    m_languageList[_T( "English (United Kingdom)" )] = _T( "en_GB" );
    m_languageList[_T( "English (New Zealand)" )] = _T( "en_NZ" );
    m_languageList[_T( "English (United States)" )] = _T( "en_US" );
    m_languageList[_T( "Esperanto (anywhere)" )] = _T( "eo_l3" );
    m_languageList[_T( "Spanish (Spain-etal)" )] = _T( "es_ES" );
    m_languageList[_T( "Spanish (Mexico)" )] = _T( "es_MX" );
    m_languageList[_T( "Faroese (Faroe Islands)" )] = _T( "fo_FO" );
    m_languageList[_T( "French (France)" )] = _T( "fr_FR" );
    m_languageList[_T( "Irish (Ireland)" )] = _T( "ga_IE" );
    m_languageList[_T( "Scottish Gaelic (Scotland)" )] = _T( "gd_GB" );
    m_languageList[_T( "Galician (Spain)" )] = _T( "gl_ES" );
    m_languageList[_T( "Hebrew (Israel)" )] = _T( "he_IL" );
    m_languageList[_T( "Croatian (Croatia)" )] = _T( "hr_HR" );
    m_languageList[_T( "Hungarian (Hungary)" )] = _T( "hu_HU" );
    m_languageList[_T( "Interlingua (x-register)" )] = _T( "ia" );
    m_languageList[_T( "Indonesian (Indonesia)" )] = _T( "id_ID" );
    m_languageList[_T( "Italian (Italy)" )] = _T( "it_IT" );
    m_languageList[_T( "Kurdish (Turkey)" )] = _T( "ku_TR" );
    m_languageList[_T( "Latin (x-register)" )] = _T( "la" );
    m_languageList[_T( "Lithuanian (Lithuania)" )] = _T( "lt_LT" );
    m_languageList[_T( "Latvian (Latvia)" )] = _T( "lv_LV" );
    m_languageList[_T( "Malagasy (Madagascar)" )] = _T( "mg_MG" );
    m_languageList[_T( "Maori (New Zealand)" )] = _T( "mi_NZ" );
    m_languageList[_T( "Malay (Malaysia)" )] = _T( "ms_MY" );
    m_languageList[_T( "Norwegian Bokmaal (Norway)" )] = _T( "nb_NO" );
    m_languageList[_T( "Dutch (Netherlands)" )] = _T( "nl_NL" );
    m_languageList[_T( "Norwegian Nynorsk (Norway)" )] = _T( "nn_NO" );
    m_languageList[_T( "Chichewa (Malawi)" )] = _T( "ny_MW" );
    m_languageList[_T( "Polish (Poland)" )] = _T( "pl_PL" );
    m_languageList[_T( "Portuguese (Brazil)" )] = _T( "pt_BR" );
    m_languageList[_T( "Portuguese (Portugal)" )] = _T( "pt_PT" );
    m_languageList[_T( "Romanian (Romania)" )] = _T( "ro_RO" );
    m_languageList[_T( "Russian (Russia)" )] = _T( "ru_RU" );
    m_languageList[_T( "Russian ye (Russia)" )] = _T( "ru_RU_ie" );
    m_languageList[_T( "Russian yo (Russia)" )] = _T( "ru_RU_yo" );
    m_languageList[_T( "Kiswahili (Africa)" )] = _T( "rw_RW" );
    m_languageList[_T( "Slovak (Slovakia)" )] = _T( "sk_SK" );
    m_languageList[_T( "Slovenian (Slovenia)" )] = _T( "sl_SI" );
    m_languageList[_T( "Swedish (Sweden)" )] = _T( "sv_SE" );
    m_languageList[_T( "Kiswahili (Africa)" )] = _T( "sw_KE" );
    m_languageList[_T( "Tetum (Indonesia)" )] = _T( "tet_ID" );
    m_languageList[_T( "Tagalog (Philippines)" )] = _T( "tl_PH" );
    m_languageList[_T( "Setswana (Africa)" )] = _T( "tn_ZA" );
    m_languageList[_T( "Ukrainian (Ukraine)" )] = _T( "uk_UA" );
    m_languageList[_T( "Zulu (Africa)" )] = _T( "zu_ZA" );
}
// ------------------------------------------------------------
void IHunSpell::GetAllLanguageKeyNames(wxArrayString& lang)
{
    lang.Clear();
    languageMap::iterator itLang;

    for(itLang = m_languageList.begin(); itLang != m_languageList.end(); ++itLang) {
        wxString key = itLang->first;
        lang.Add(key);
    }
}
// ------------------------------------------------------------
void IHunSpell::GetAvailableLanguageKeyNames(const wxString& path, wxArrayString& lang)
{
    lang.Clear();
    languageMap::iterator itLang;

    for(itLang = m_languageList.begin(); itLang != m_languageList.end(); ++itLang) {
        wxFileName fna(path, "");
        wxFileName fnd(path, "");

        fna.SetName(itLang->second);
        fna.SetExt("aff");

        fnd.SetName(itLang->second);
        fnd.SetExt("dic");

        if(!fna.FileExists() || !fnd.FileExists())
            continue;

        lang.Add(itLang->first);
    }
}

// ------------------------------------------------------------
void IHunSpell::EnableScannerType(int type, bool state)
{
    if(state)
        m_scanners |= type;
    else
        m_scanners &= ~type;
}
// ------------------------------------------------------------
int IHunSpell::CheckCppType(IEditor* pEditor)
{
    wxStringTokenizer tkz;

    int retVal = kNoSpellingError;
    int offset = 0;

    for(wxUint32 i = 0; i < m_parseValues.size(); i++) {
        posLen pl = m_parseValues[i].first;
        wxString text = pEditor->GetTextRange(pl.first, pl.second);
        wxString del = s_commentDelimiters;

        if(m_parseValues[i].second ==
           kString) { // replace \n\r\t in strings with blanks to correctly tokenize content like '\nNext line'
            wxRegEx re(s_wsRegEx, wxRE_ADVANCED);
            // to ensure that \\n will not get captured by the regex, we temporarily replace it
            text.Replace(s_DOUBLE_BACKSLASH, s_PLACE_HOLDER);

            if(re.Matches(text)) {
                re.ReplaceAll(&text, wxT("  "));
                del = s_cppDelimiters;
            }

            // restore
            text.Replace(s_PLACE_HOLDER, s_DOUBLE_BACKSLASH);
        }

        tkz.SetString(text, del);

        while(tkz.HasMoreTokens()) {
            wxString token = tkz.GetNextToken();
            int pos = pl.first + tkz.GetPosition() - token.Len() - 1;
            pos += offset;

            if(token.Len() <= MIN_TOKEN_LEN)
                continue;

            if(m_parseValues[i].second == kString) { // ignore filenames in #include
                wxString line = pEditor->GetCtrl()->GetLine(pEditor->LineFromPos(pl.first));

                if(line.Find(s_include) != wxNOT_FOUND)
                    continue;
            }

            // Note checking tags database only in continuous mode.
            if(!CheckWord(token)) {
                pEditor->SetUserIndicator(pos, token.Len());
                pEditor->SetCaretAt(pos);
                pEditor->SelectText(pos, token.Len());
                // show correct spelling dialog
                retVal = kSpellingError;
                m_pSpellDlg->SetMisspelled(token);
                m_pSpellDlg->SetSuggestions(GetSuggestions(token));
                int ret = m_pSpellDlg->ShowModal();

                switch(ret) {
                case SC_CHANGE: {
                    // correct spelling
                    wxString replace = m_pSpellDlg->GetMisspelled();
                    offset += replace.Len() - token.Len();
                    text.replace(tkz.GetPosition(), token.Len(), replace);
                    pEditor->ReplaceSelection(replace);
                } break;
                case SC_IGNORE:
                    AddWordToIgnoreList(token);
                    break;
                case SC_ADD:
                    AddWordToUserDict(token);
                    break;
                default: {
                    pEditor->ClearUserIndicators();
                    return kSpellingCanceled;
                }
                }
            }
        }
    }
    return retVal;
}
// ------------------------------------------------------------
int IHunSpell::MarkErrors(IEditor* pEditor)
{
    wxStringTokenizer tkz;

    int counter = 0;
    pEditor->ClearUserIndicators();

    for(wxUint32 i = 0; i < m_parseValues.size(); i++) {
        posLen pl = m_parseValues[i].first;
        wxString text = pEditor->GetTextRange(pl.first, pl.second);
        wxString del = s_commentDelimiters;

        if(m_parseValues[i].second ==
           kString) { // replace \n\r\t in strings with blanks to correctly tokenize content like '\nNext line'
            wxRegEx re(s_wsRegEx, wxRE_ADVANCED);
            // to ensure that \\n will not get captured by the regex, we temporarily replace it
            text.Replace(s_DOUBLE_BACKSLASH, s_PLACE_HOLDER);
            if(re.Matches(text)) {
                re.ReplaceAll(&text, wxT("  "));
                del = s_cppDelimiters;
            }
            text.Replace(s_PLACE_HOLDER, s_DOUBLE_BACKSLASH);
        }
        tkz.SetString(text, del);

        while(tkz.HasMoreTokens()) {
            wxString token = tkz.GetNextToken();
            int pos = pl.first + tkz.GetPosition() - token.Len() - 1;

            if(token.Len() <= MIN_TOKEN_LEN)
                continue;

            if(m_parseValues[i].second == kString) {
                wxString line = pEditor->GetCtrl()->GetLine(pEditor->LineFromPos(pl.first));

                if(line.Find(s_include) != wxNOT_FOUND) // ignore filenames
                    continue;
            }

            if(!CheckWord(token) && !IsTag(token)) {
                pEditor->SetUserIndicator(pos, token.Len());
                counter++;
            }
        }
    }

    return counter;
}

void IHunSpell::SetCaseSensitiveUserDictionary(const bool caseSensitiveUserDictionary)
{
    if(caseSensitiveUserDictionary != m_caseSensitiveUserDictionary) {
        m_caseSensitiveUserDictionary = caseSensitiveUserDictionary;

        // Re-order user dictionary and ignores.
        CustomDictionary userDict(m_userDict.begin(), m_userDict.end(), 0,
                                  StringHashOptionalCase(caseSensitiveUserDictionary),
                                  StringCompareOptionalCase(caseSensitiveUserDictionary));
        m_userDict.swap(userDict);

        CustomDictionary ignoreList(m_ignoreList.begin(), m_ignoreList.end(), 0,
                                    StringHashOptionalCase(caseSensitiveUserDictionary),
                                    StringCompareOptionalCase(caseSensitiveUserDictionary));
        m_ignoreList.swap(ignoreList);
    }
}

void IHunSpell::AddWord(const wxString& word)
{
#if wxUSE_STL
    // Implicit conversions are disabled when building with wxUSE_STL=1
    Hunspell_add(m_pSpell, word.mb_str().data());
#else
    Hunspell_add(m_pSpell, word);
#endif
}
// ------------------------------------------------------------
