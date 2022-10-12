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

#include "clLocaleManager.hpp"
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
#include <wx/msgdlg.h>
#include <wx/wxprec.h>

#ifdef __WXMSW__
#include <wx/msw/uxtheme.h>
#endif

EditorSettingsMiscPanel::EditorSettingsMiscPanel(wxWindow* parent, OptionsConfigPtr options)
    : OptionsConfigPage(parent, options)
{
    AddHeader(_("General"));
    AddProperty(_("Allow only single instance"), clConfig::Get().Read(kConfigSingleInstance, false),
                UPDATE_CLCONFIG_BOOL_CB(kConfigSingleInstance));
    AddProperty(_("Web search URL"), m_options->GetWebSearchPrefix(), UPDATE_TEXT_CB(SetWebSearchPrefix));
#ifdef __WXMSW__
    AddProperty(_("Use Direct2D editor drawings"), clConfig::Get().Read("Editor/UseDirect2D", true),
                UPDATE_CLCONFIG_BOOL_CB("Editor/UseDirect2D"));
#endif
    AddProperty(_("Frame title"), clConfig::Get().Read(kConfigFrameTitlePattern, wxString("$workspace $fullpath")),
                UPDATE_CLCONFIG_TEXT_CB(kConfigFrameTitlePattern));

    AddHeader(_("Startup"));
    AddProperty(_("Check for new version on startup"), clConfig::Get().Read(kConfigCheckForNewVersion, true),
                UPDATE_CLCONFIG_BOOL_CB(kConfigCheckForNewVersion));
    AddProperty(_("Only prompt for stable releases"), clConfig::Get().Read("PromptForNewReleaseOnly", false),
                UPDATE_CLCONFIG_BOOL_CB("PromptForNewReleaseOnly"));
    AddProperty(_("Load last session on startup"), clConfig::Get().Read(kConfigRestoreLastSession, true),
                UPDATE_CLCONFIG_BOOL_CB(kConfigRestoreLastSession));
    AddProperty(_("Show splashscreen"), clConfig::Get().Read("ShowSplashScreen", true),
                UPDATE_CLCONFIG_BOOL_CB("ShowSplashScreen"));

    AddHeader(_("Status bar"));
    AddProperty(_("Show current line number"), clConfig::Get().Read(kConfigStatusbarShowLine, true),
                UPDATE_CLCONFIG_BOOL_CB(kConfigStatusbarShowLine));
    AddProperty(_("Show current column"), clConfig::Get().Read(kConfigStatusbarShowColumn, true),
                UPDATE_CLCONFIG_BOOL_CB(kConfigStatusbarShowColumn));
    AddProperty(_("Show current position"), clConfig::Get().Read(kConfigStatusbarShowPosition, true),
                UPDATE_CLCONFIG_BOOL_CB(kConfigStatusbarShowPosition));
    AddProperty(_("Show file length"), clConfig::Get().Read(kConfigStatusbarShowLength, false),
                UPDATE_CLCONFIG_BOOL_CB(kConfigStatusbarShowLength));
    AddProperty(_("Show number of selected chars"), clConfig::Get().Read(kConfigStatusbarShowSelectedChars, true),
                UPDATE_CLCONFIG_BOOL_CB(kConfigStatusbarShowSelectedChars));

    AddHeader(_("Tool bar"));
    AddProperty(_("Space between button groups"), clConfig::Get().Read(kConfigToolbarGroupSpacing, 30),
                UPDATE_CLCONFIG_INT_CB(kConfigToolbarGroupSpacing));
    wxArrayString locales;
    int selected_locale = FindAvailableLocales(&locales);
    wxArrayString astrEncodings;
    wxFontEncoding fontEnc;
    int selected_encoding = 0;
    size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
    for(size_t i = 0; i < iEncCnt; i++) {
        fontEnc = wxFontMapper::GetEncoding(i);
        if(wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
            continue;
        }
        astrEncodings.Add(wxFontMapper::GetEncodingName(fontEnc));
        if(fontEnc == options->GetFileFontEncoding()) {
            selected_encoding = i;
        }
    }

    AddHeader(_("Locale"));
    AddProperty(_("File text encoding"), astrEncodings, selected_encoding, UPDATE_TEXT_CB(SetFileFontEncoding));
    AddProperty(_("Enable localization"), m_options->GetUseLocale(), UPDATE_BOOL_CB(SetUseLocale));
    AddProperty(_("Locale to use"), locales, selected_locale, UPDATE_TEXT_CB(SetPreferredLocale));

    AddHeader(_("Log"));

    std::vector<wxString> log_levels = { FileLogger::GetVerbosityAsString(FileLogger::System),
                                         FileLogger::GetVerbosityAsString(FileLogger::Error),
                                         FileLogger::GetVerbosityAsString(FileLogger::Warning),
                                         FileLogger::GetVerbosityAsString(FileLogger::Dbg),
                                         FileLogger::GetVerbosityAsString(FileLogger::Developer) };

    int cur_log = clConfig::Get().Read(kConfigLogVerbosity, FileLogger::Error);

    AddProperty(_("Log file verbosity"), log_levels, FileLogger::GetVerbosityAsString(cur_log),
                [](const wxString&, const wxAny& value) {
                    wxString str;
                    if(value.GetAs(&str)) {
                        clConfig::Get().Write(kConfigLogVerbosity, FileLogger::GetVerbosityAsNumber(str));
                        FileLogger::SetVerbosity(FileLogger::GetVerbosityAsNumber(str));
                    }
                });
#ifdef __WXGTK__
    AddProperty(_("Redirect stdout/stderr to file"), clConfig::Get().Read(kConfigRedirectLogOutput, true),
                UPDATE_CLCONFIG_BOOL_CB(kConfigRedirectLogOutput));
#endif

    AddHeader(_("History"));
    AddPropertyButton(_("Forget recent workspaces and files"), _("Clear ") + wxT("\u232B"),
                      [](const wxString&, const wxAny& value) {
                          wxUnusedVar(value);
                          ManagerST::Get()->ClearWorkspaceHistory();
                          clMainFrame::Get()->GetMainBook()->ClearFileHistory();
                          wxMessageBox(_("Success!"));
                      });

    AddPropertyButton(_("Reset annoying dialogs answers"), _("Clear ") + wxT("\u232B"),
                      [](const wxString&, const wxAny& value) {
                          wxUnusedVar(value);
                          clConfig::Get().ClearAnnoyingDlgAnswers();
                          wxMessageBox(_("Success!"));
                      });
}

int EditorSettingsMiscPanel::FindAvailableLocales(wxArrayString* locales)
{
    const wxArrayString& cached_locales = clLocaleManager::get().GetCachedLocales();
    int system_default_locale = clLocaleManager::get().GetSystemDefaultLocale();
    wxString preffered_locale = m_options->GetPreferredLocale();

    int select = wxNOT_FOUND;
    // find the selection
    for(size_t i = 0; i < cached_locales.size(); ++i) {
        if(cached_locales[i] == preffered_locale) {
            select = static_cast<int>(i);
            break;
        }
    }

    locales->reserve(cached_locales.size());
    locales->insert(locales->end(), cached_locales.begin(), cached_locales.end());
    return (select != wxNOT_FOUND) ? select : system_default_locale;
}
