//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingsmiscpanel.cpp
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

#include "editorsettingsmiscpanel.h"

#include "cl_config.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "frame.h"
#include "generalinfo.h"
#include "globals.h"
#include "manager.h"
#include "pluginmanager.h"

#include <wx/fontmap.h>
#include <wx/intl.h>
#include <wx/wxprec.h>

#ifdef __WXMSW__
#include <wx/msw/uxtheme.h>
#endif

EditorSettingsMiscPanel::EditorSettingsMiscPanel(wxWindow* parent)
    : EditorSettingsMiscBasePanel(parent)
    , TreeBookNode<EditorSettingsMiscPanel>()
    , m_restartRequired(false)
{
    GeneralInfo info = clMainFrame::Get()->GetFrameGeneralInfo();
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    if(options->GetIconsSize() == 16) {
        m_toolbarIconSize->SetSelection(0);
    } else {
        m_toolbarIconSize->SetSelection(1);
    }

    m_oldSetLocale = options->GetUseLocale();
    m_SetLocale->SetValue(m_oldSetLocale);
    m_oldpreferredLocale = options->GetPreferredLocale();
    // Load the available locales and feed them to the wxchoice
    int select = FindAvailableLocales();
    if(select != wxNOT_FOUND) {
        m_AvailableLocales->SetSelection(select);
    }

    wxArrayString astrEncodings;
    wxFontEncoding fontEnc;
    int iCurrSelId = 0;
    size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
    for(size_t i = 0; i < iEncCnt; i++) {
        fontEnc = wxFontMapper::GetEncoding(i);
        if(wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
            continue;
        }
        astrEncodings.Add(wxFontMapper::GetEncodingName(fontEnc));
        if(fontEnc == options->GetFileFontEncoding()) {
            iCurrSelId = i;
        }
    }

    m_fileEncoding->Append(astrEncodings);
    m_fileEncoding->SetSelection(iCurrSelId);

    m_singleAppInstance->SetValue(clConfig::Get().Read(kConfigSingleInstance, false));
    m_versionCheckOnStartup->SetValue(clConfig::Get().Read(kConfigCheckForNewVersion, true));
    m_maxItemsFindReplace->ChangeValue(::wxIntToString(clConfig::Get().Read(kConfigMaxItemsInFindReplaceDialog, 15)));
    m_spinCtrlMaxOpenTabs->ChangeValue(::wxIntToString(clConfig::Get().Read(kConfigMaxOpenedTabs, 15)));
    m_choice4->SetStringSelection(
        FileLogger::GetVerbosityAsString(clConfig::Get().Read(kConfigLogVerbosity, FileLogger::Error)));
    m_checkBoxRestoreSession->SetValue(clConfig::Get().Read(kConfigRestoreLastSession, true));
    m_textCtrlPattern->ChangeValue(clConfig::Get().Read(kConfigFrameTitlePattern, wxString("$workspace $fullpath")));
    m_statusbarShowLine->SetValue(clConfig::Get().Read(kConfigStatusbarShowLine, true));
    m_statusbarShowCol->SetValue(clConfig::Get().Read(kConfigStatusbarShowColumn, true));
    m_statusbarShowPos->SetValue(clConfig::Get().Read(kConfigStatusbarShowPosition, false));
    m_statusbarShowFileLength->SetValue(clConfig::Get().Read(kConfigStatusbarShowLength, false));
    m_statusBarShowSelChars->SetValue(clConfig::Get().Read(kConfigStatusbarShowSelectedChars, true));
    m_textCtrlSeparation->ChangeValue(::wxIntToString(clConfig::Get().Read(kConfigToolbarGroupSpacing, 30)));

    m_redirectLogOutput->SetValue(clConfig::Get().Read(kConfigRedirectLogOutput, true));
    m_checkBoxPromptReleaseOnly->SetValue(clConfig::Get().Read("PromptForNewReleaseOnly", false));

    GetWebSearchPrefix()->ChangeValue(options->GetWebSearchPrefix());
    m_checkBoxDirect2D->SetValue(clConfig::Get().Read("Editor/UseDirect2D", true));
}

void EditorSettingsMiscPanel::OnClearButtonClick(wxCommandEvent&)
{
    ManagerST::Get()->ClearWorkspaceHistory();
    clMainFrame::Get()->GetMainBook()->ClearFileHistory();
}

void EditorSettingsMiscPanel::Save(OptionsConfigPtr options)
{
    clConfig::Get().Write(kConfigSingleInstance, m_singleAppInstance->IsChecked());
    clConfig::Get().Write(kConfigCheckForNewVersion, m_versionCheckOnStartup->IsChecked());
    clConfig::Get().Write(kConfigMaxItemsInFindReplaceDialog, ::wxStringToInt(m_maxItemsFindReplace->GetValue(), 15));
    clConfig::Get().Write(kConfigMaxOpenedTabs, ::wxStringToInt(m_spinCtrlMaxOpenTabs->GetValue(), 15));
    clConfig::Get().Write(kConfigRestoreLastSession, m_checkBoxRestoreSession->IsChecked());
    clConfig::Get().Write(kConfigFrameTitlePattern, m_textCtrlPattern->GetValue());
    clConfig::Get().Write(kConfigStatusbarShowLine, m_statusbarShowLine->IsChecked());
    clConfig::Get().Write(kConfigStatusbarShowColumn, m_statusbarShowCol->IsChecked());
    clConfig::Get().Write(kConfigStatusbarShowPosition, m_statusbarShowPos->IsChecked());
    clConfig::Get().Write(kConfigStatusbarShowLength, m_statusbarShowFileLength->IsChecked());
    clConfig::Get().Write(kConfigStatusbarShowSelectedChars, m_statusBarShowSelChars->IsChecked());
    clConfig::Get().Write(kConfigToolbarGroupSpacing, ::wxStringToInt(m_textCtrlSeparation->GetValue(), 30));

    // check to see of the icon size was modified
    int oldIconSize(24);

    OptionsConfigPtr oldOptions = EditorConfigST::Get()->GetOptions();
    if(oldOptions) {
        oldIconSize = oldOptions->GetIconsSize();
    }

    int iconSize(24);
    if(m_toolbarIconSize->GetSelection() == 0) {
        iconSize = 16;
    }
    options->SetIconsSize(iconSize);

    bool setlocale = m_SetLocale->IsChecked();
    options->SetUseLocale(setlocale);
    wxString newLocaleString = m_AvailableLocales->GetStringSelection();
    // I don't think we should check if newLocaleString is empty; that's still useful information
    newLocaleString = newLocaleString.BeforeFirst(':'); // Store it as "fr_FR", not "fr_FR: French"
    options->SetPreferredLocale(newLocaleString);
    if((setlocale != m_oldSetLocale) || (newLocaleString != m_oldpreferredLocale)) {
        m_restartRequired = true;
    }

    // save file font encoding
    options->SetFileFontEncoding(m_fileEncoding->GetStringSelection());
    TagsManagerST::Get()->SetEncoding(options->GetFileFontEncoding());

    if(oldIconSize != iconSize) {
        EditorConfigST::Get()->SetInteger("LoadSavedPrespective", 0);
        // notify the user
        m_restartRequired = true;
    } else {
        EditorConfigST::Get()->SetInteger("LoadSavedPrespective", 1);
    }

    size_t flags = options->GetOptions();

    // Clear old settings
    flags &= ~(OptionsConfig::Opt_IconSet_Classic);
    flags &= ~(OptionsConfig::Opt_IconSet_FreshFarm);
    flags &= ~(OptionsConfig::Opt_IconSet_Classic_Dark);
    flags |= OptionsConfig::Opt_IconSet_Classic;

    clConfig::Get().Write("RedirectLogOutput", m_redirectLogOutput->IsChecked());
    clConfig::Get().Write("PromptForNewReleaseOnly", m_checkBoxPromptReleaseOnly->IsChecked());
    options->SetOptions(flags);
    options->SetWebSearchPrefix(GetWebSearchPrefix()->GetValue());
    bool useDirect2D_Old = clConfig::Get().Read("Editor/UseDirect2D", true);
    clConfig::Get().Write("Editor/UseDirect2D", m_checkBoxDirect2D->IsChecked());
    if(useDirect2D_Old != m_checkBoxDirect2D->IsChecked()) {
        m_restartRequired = true;
    }
}

void EditorSettingsMiscPanel::OnClearUI(wxUpdateUIEvent& e)
{
    wxArrayString a1, a2;
    clMainFrame::Get()->GetMainBook()->GetRecentlyOpenedFiles(a1);
    ManagerST::Get()->GetRecentlyOpenedWorkspaces(a2);
    e.Enable(!a1.IsEmpty() && !a2.IsEmpty());
}

void EditorSettingsMiscPanel::OnEnableThemeUI(wxUpdateUIEvent& event) { event.Enable(false); }

void EditorSettingsMiscPanel::LocaleChkUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(m_AvailableLocales->GetCount() > 0);
}

void EditorSettingsMiscPanel::LocaleChoiceUpdateUI(wxUpdateUIEvent& event) { event.Enable(m_SetLocale->IsChecked()); }

void EditorSettingsMiscPanel::LocaleStaticUpdateUI(wxUpdateUIEvent& event) { event.Enable(m_SetLocale->IsChecked()); }

int EditorSettingsMiscPanel::FindAvailableLocales()
{
    wxArrayString canonicalNames;
    int select(wxNOT_FOUND), sysdefault_sel(wxNOT_FOUND);
    m_AvailableLocales->Clear();

    int system_lang = wxLocale::GetSystemLanguage();
    if(system_lang == wxLANGUAGE_UNKNOWN) {
        // Least-stupid fallback value
        system_lang = wxLANGUAGE_ENGLISH_US;
    }

    for(int n = 0, lang = wxLANGUAGE_UNKNOWN + 1; lang < wxLANGUAGE_USER_DEFINED; ++lang) {
        const wxLanguageInfo* info = wxLocale::GetLanguageInfo(lang);
        // Check there *is* a Canonical name, as empty strings return a valid locale :/
        if((info && !info->CanonicalName.IsEmpty()) && wxLocale::IsAvailable(lang)) {

            // Check we haven't already seen this item: we may find the system default twice
            if(canonicalNames.Index(info->CanonicalName) == wxNOT_FOUND) {
                // Display the name as e.g. "en_GB: English (U.K.)"
                m_AvailableLocales->Append(info->CanonicalName + ": " + info->Description);
                canonicalNames.Add(info->CanonicalName);

                if(info->CanonicalName == m_oldpreferredLocale) {
                    // Use this as the selection in the wxChoice
                    select = n;
                }
                if(lang == system_lang) {
                    // Use this as the selection if m_oldpreferredLocale isn't found
                    sysdefault_sel = n;
                }
                ++n;
            }
        }
    }

    return (select != wxNOT_FOUND) ? select : sysdefault_sel;
}

void EditorSettingsMiscPanel::OnLogVerbosityChanged(wxCommandEvent& event)
{
    FileLogger::SetVerbosity(event.GetString());
    clConfig::Get().Write("LogVerbosity", FileLogger::GetVerbosityAsNumber(m_choice4->GetStringSelection()));
}

void EditorSettingsMiscPanel::OnShowLogFile(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString logfile;
    logfile << clStandardPaths::Get().GetUserDataDir() << wxFileName::GetPathSeparator() << "codelite.log";

    clMainFrame::Get()->GetMainBook()->OpenFile(logfile);
}

void EditorSettingsMiscPanel::OnLogoutputCheckUpdateUI(wxUpdateUIEvent& event)
{
#ifdef __WXGTK__
    event.Enable(true);
#else
    m_redirectLogOutput->SetValue(false);
    event.Enable(false);
#endif
}

void EditorSettingsMiscPanel::OnResetAnnoyingDialogsAnswers(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clConfig::Get().ClearAnnoyingDlgAnswers();
}

void EditorSettingsMiscPanel::OnPromptStableReleaseUI(wxUpdateUIEvent& event)
{
    event.Enable(m_versionCheckOnStartup->IsChecked());
}

void EditorSettingsMiscPanel::OnUseDirect2DUI(wxUpdateUIEvent& event)
{
#ifdef __WXMSW__
    event.Enable(true);
#else
    event.Enable(false);
    event.Check(false);
#endif
}
