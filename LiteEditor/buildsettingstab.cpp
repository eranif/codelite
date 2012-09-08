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
#include "frame.h"
#include "buidltab.h"
#include "buildsettingstab.h"
#include "buildtabsettingsdata.h"
#include "editor_config.h"

BuildTabSetting::BuildTabSetting( wxWindow* parent )
    : BuildTabSettingsBase( parent )
{
    BuildTabSettingsData options;
    EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);
    m_checkBoxSkipWarnings->SetValue( options.GetSkipWarnings() );
    m_colourPickerErrorFg->SetColour(options.GetErrorColour());
    m_colourPickerWarningsFg->SetColour(options.GetWarnColour());
    m_checkBoxBoldErrFont->SetValue(options.GetBoldErrFont());
    m_checkBoxBoldWarnFont->SetValue(options.GetBoldWarnFont());
    m_radioBoxShowBuildTab->Select(options.GetShowBuildPane());
    m_checkBoxAutoHide->SetValue(options.GetAutoHide());
    m_radioBuildPaneScrollDestination->SetSelection(options.GetBuildPaneScrollDestination());
    m_checkBoxDisplayMarkers->SetValue(options.GetErrorWarningStyle() & BuildTabSettingsData::EWS_Bookmarks);
}

void BuildTabSetting::Save()
{
    BuildTabSettingsData options;
    options.SetErrorColour(m_colourPickerErrorFg->GetColour().GetAsString(wxC2S_HTML_SYNTAX));
    options.SetWarnColour(m_colourPickerWarningsFg->GetColour().GetAsString(wxC2S_HTML_SYNTAX));
    options.SetSkipWarnings(m_checkBoxSkipWarnings->IsChecked());
    options.SetBoldErrFont(m_checkBoxBoldErrFont->IsChecked());
    options.SetBoldWarnFont(m_checkBoxBoldWarnFont->IsChecked());
    options.SetShowBuildPane(m_radioBoxShowBuildTab->GetSelection());
    options.SetAutoHide(m_checkBoxAutoHide->IsChecked());
    options.SetBuildPaneScrollDestination(m_radioBuildPaneScrollDestination->GetSelection());

    int flag (BuildTabSettingsData::EWS_NoMarkers);
    if ( m_checkBoxDisplayMarkers->IsChecked() ) {
        flag |= BuildTabSettingsData::EWS_Bookmarks;
    }

    options.SetErrorWarningStyle( flag );
    EditorConfigST::Get()->WriteObject(wxT("build_tab_settings"), &options);
}

void BuildTabSetting::OnUpdateUI(wxUpdateUIEvent& event)
{
}
