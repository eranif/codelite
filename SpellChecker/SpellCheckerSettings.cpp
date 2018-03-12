//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SpellCheckerSettings.cpp
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
// Name:        SpellCheckerSettings.cpp
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     02/02/14
// SVN-ID:      $Id: SpellCheckerSettings.cpp 35 2014-02-22 18:18:49Z Frank $
// Copyright:   2014 Frank Lichtner
// License:
/////////////////////////////////////////////////////////////////////////////
#include <wx/valgen.h>
#include "SpellCheckerSettings.h"
#include "IHunSpell.h"
#include <windowattrmanager.h>

// ------------------------------------------------------------
SpellCheckerSettings::SpellCheckerSettings(wxWindow* parent)
    : SpellCheckerSettings_base(parent)
{
    m_pHunspell = NULL;
    m_dictionaryPath.Empty();

    m_pStrings->SetValidator(wxGenericValidator(&m_scanStrings));
    m_pCppComments->SetValidator(wxGenericValidator(&m_scanCPP));
    m_pC_Comments->SetValidator(wxGenericValidator(&m_scanC));
    m_pDox1->SetValidator(wxGenericValidator(&m_scanD1));
    m_pDox2->SetValidator(wxGenericValidator(&m_scanD2));
    m_pCurrentLanguage->SetValidator(wxGenericValidator(&m_dictionaryFileName));
    m_pDirPicker->GetTextCtrl()->SetEditable(false);
    m_pDirPicker->GetTextCtrl()->SetBackgroundColour(wxColour(255, 255, 230));
    SetName("SpellCheckerSettings");
    WindowAttrManager::Load(this);
}

// ------------------------------------------------------------
void SpellCheckerSettings::OnInitDialog(wxInitDialogEvent& event)
{
    event.Skip();

	m_pCaseSensitiveUserDictionary->SetValue(m_caseSensitiveUserDictionary);
	m_pIgnoreSymbolsInTagsDatabase->SetValue(m_ignoreSymbolsInTagsDatabase);

    if(m_pHunspell) {
        m_pDirPicker->SetPath(m_dictionaryPath);

        if(!m_dictionaryPath.IsEmpty()) FillLanguageList();
    }
}
// ------------------------------------------------------------
void SpellCheckerSettings::OnLanguageSelected(wxCommandEvent& event)
{
    if(m_pHunspell) {
        wxString key = m_pLanguageList->GetString(event.GetInt());
        m_pCurrentLanguage->SetValue(m_pHunspell->GetLanguageShort(key));
    }
}
// ------------------------------------------------------------
void SpellCheckerSettings::OnUpdateOk(wxUpdateUIEvent& event)
{
    int checked = 0;

    if(m_pStrings->IsChecked()) checked++;

    if(m_pCppComments->IsChecked()) checked++;

    if(m_pC_Comments->IsChecked()) checked++;

    if(m_pDox1->IsChecked()) checked++;

    if(m_pDox2->IsChecked()) checked++;

    if(checked && !m_pCurrentLanguage->GetValue().IsEmpty())
        event.Enable(true);
    else
        event.Enable(false);
}
// ------------------------------------------------------------
void SpellCheckerSettings::OnOk(wxCommandEvent& event)
{
    event.Skip();
    m_dictionaryPath = m_pDirPicker->GetPath();
	m_caseSensitiveUserDictionary = m_pCaseSensitiveUserDictionary->GetValue();
    m_ignoreSymbolsInTagsDatabase = m_pIgnoreSymbolsInTagsDatabase->GetValue();

    if(!wxEndsWithPathSeparator(m_dictionaryPath)) m_dictionaryPath += wxFILE_SEP_PATH;
    ;
}
// ------------------------------------------------------------
void SpellCheckerSettings::OnDirChanged(wxFileDirPickerEvent& event)
{
    m_dictionaryPath = m_pDirPicker->GetPath();

    if(!wxEndsWithPathSeparator(m_dictionaryPath)) m_dictionaryPath += wxFILE_SEP_PATH;
    m_pLanguageList->Clear();
    m_pCurrentLanguage->SetValue(wxT(""));
    FillLanguageList();
}
// ------------------------------------------------------------
void SpellCheckerSettings::FillLanguageList()
{
    if(!m_dictionaryPath.IsEmpty()) {
        wxArrayString lang;
        m_pHunspell->GetAvailableLanguageKeyNames(m_dictionaryPath, lang);
        m_pLanguageList->Clear();
        m_pLanguageList->Append(lang);
    }
}
// ------------------------------------------------------------
void SpellCheckerSettings::OnClearIgnoreList(wxCommandEvent& event) { m_pHunspell->ClearIgnoreList(); }

SpellCheckerSettings::~SpellCheckerSettings() {}

void SpellCheckerSettings::SetDictionaryPath(const wxString& dictionaryPath)
{
    this->m_dictionaryPath = dictionaryPath;
    FillLanguageList();
}

// ------------------------------------------------------------
