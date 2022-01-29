//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : buildsettingstab.cpp
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
#include "buildsettingstab.h"

#include "buildtabsettingsdata.h"
#include "clFontHelper.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "frame.h"
#include "globals.h"
#include "macros.h"

#include <wx/any.h>
#include <wx/fontdlg.h>

BuildTabSetting::BuildTabSetting(wxWindow* parent)
    : BuildTabSettingsBase(parent)
    , m_isModified(false)
{
    BuildTabSettingsData options;
    EditorConfigST::Get()->ReadObject("BuildTabSettings", &options);
    m_choiceMarkerStyle->SetSelection(static_cast<int>(options.GetErrorWarningStyle()));
    m_checkBoxScrollToError->SetValue(options.GetScrollTo() == BuildTabSettingsData::SCROLL_TO_FIRST_ERROR);
    m_checkBoxSkipWarnings->SetValue(options.IsSkipWarnings());
}

void BuildTabSetting::Save()
{
    BuildTabSettingsData options;
    options.SetSkipWarnings(m_checkBoxSkipWarnings->IsChecked());

    wxString marker_style = m_choiceMarkerStyle->GetStringSelection().Lower();
    if(marker_style == "none") {
        options.SetErrorWarningStyle(BuildTabSettingsData::MARKER_NONE);
    } else if(marker_style == "annotation box") {
        options.SetErrorWarningStyle(BuildTabSettingsData::MARKER_ANNOTATE);
    } else {
        options.SetErrorWarningStyle(BuildTabSettingsData::MARKER_BOOKMARKS);
    }

    options.SetScrollTo(m_checkBoxScrollToError->IsChecked() ? BuildTabSettingsData::SCROLL_TO_FIRST_ERROR
                                                             : BuildTabSettingsData::SCROLL_TO_NOWHERE);
    EditorConfigST::Get()->WriteObject(wxT("BuildTabSettings"), &options);
    m_isModified = false;
}

void BuildTabSetting::OnUpdateUI(wxUpdateUIEvent& event) { wxUnusedVar(event); }
void BuildTabSetting::OnChange(wxCommandEvent& event)
{
    event.Skip();
    m_isModified = true;
}
