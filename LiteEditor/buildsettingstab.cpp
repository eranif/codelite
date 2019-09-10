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
#include "buildsettingstab.h"
#include "buildtabsettingsdata.h"
#include "editor_config.h"
#include "new_build_tab.h"
#include <wx/any.h>
#include "macros.h"
#include <wx/fontdlg.h>
#include "clFontHelper.h"
#include "event_notifier.h"
#include "globals.h"

BuildTabSetting::BuildTabSetting(wxWindow* parent)
    : BuildTabSettingsBase(parent)
    , m_isModified(false)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
    BuildTabSettingsData options;
    EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);
    m_pgPropJumpWarnings->SetValue(options.GetSkipWarnings());

    wxVariant errorColour, warningColour;
    errorColour << wxColour(options.GetErrorColour());
    warningColour << wxColour(options.GetWarnColour());

    m_pgPropErrorColour->SetValue(errorColour);
    m_pgPropWarningColour->SetValue(warningColour);

    m_pgPropAutoShowBuildPane->SetValueFromInt(options.GetShowBuildPane());
    m_pgPropAutoHideBuildPane->SetValue((bool)options.GetAutoHide());
    m_pgPropAutoScroll->SetValueFromInt(options.GetBuildPaneScrollDestination());
    m_pgPropUseMarkers->SetValue((bool)(options.GetErrorWarningStyle() & BuildTabSettingsData::EWS_Bookmarks));
    m_pgPropUseAnnotations->SetValue((bool)(options.GetErrorWarningStyle() & BuildTabSettingsData::EWS_Annotate));
}

void BuildTabSetting::Save()
{
    BuildTabSettingsData options;

    wxColour defaultErrorColour(*wxRED);
    wxColour defaultWarningColour("rgb(128, 128, 0)");

    wxColourPropertyValue errorColour, warningColour;
    errorColour << m_pgPropErrorColour->GetValue();
    warningColour << m_pgPropWarningColour->GetValue();

    options.SetErrorColour(errorColour.m_colour.GetAsString(wxC2S_HTML_SYNTAX));
    options.SetWarnColour(warningColour.m_colour.GetAsString(wxC2S_HTML_SYNTAX));
    options.SetSkipWarnings(m_pgPropJumpWarnings->GetValue().GetBool());
    options.SetShowBuildPane(m_pgPropAutoShowBuildPane->GetValue().GetInteger());
    options.SetAutoHide(m_pgPropAutoHideBuildPane->GetValue().GetBool());
    options.SetBuildPaneScrollDestination(m_pgPropAutoScroll->GetValue().GetInteger());

    int flag(BuildTabSettingsData::EWS_NoMarkers);
    if(m_pgPropUseMarkers->GetValue().GetBool()) { flag |= BuildTabSettingsData::EWS_Bookmarks; }

    if(m_pgPropUseAnnotations->GetValue().GetBool()) { flag |= BuildTabSettingsData::EWS_Annotate; }

    options.SetErrorWarningStyle(flag);
    EditorConfigST::Get()->WriteObject(wxT("build_tab_settings"), &options);
    m_isModified = false;
}

void BuildTabSetting::OnUpdateUI(wxUpdateUIEvent& event) {}

void BuildTabSetting::OnCustomButtonClicked(wxCommandEvent& event)
{
    wxPGProperty* prop = m_pgMgr->GetSelectedProperty();
    CHECK_PTR_RET(prop);
}

void BuildTabSetting::SelectFont() {}
void BuildTabSetting::OnAppearanceChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    m_isModified = true;
}
