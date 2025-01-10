//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : editorsettingsdockingwidows.cpp
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

#include "editorsettingsdockingwidows.h"

#include "imanager.h"

EditorSettingsDockingWindows::EditorSettingsDockingWindows(wxWindow* parent, OptionsConfigPtr options)
    : OptionsConfigPage(parent, options)
{
    AddHeader(_("Tabs"));

    const std::unordered_map<wxString, int> tab_height_map = { { "TALL", OptionsConfig::nbTabHt_Tall },
                                                               { "MEDIUM", OptionsConfig::nbTabHt_Medium },
                                                               { "SHORT", OptionsConfig::nbTabHt_Short },
                                                               { "TINY", OptionsConfig::nbTabHt_Tiny } };

    std::unordered_map<int, wxString> tab_height_map_reverse = { { OptionsConfig::nbTabHt_Tall, "TALL" },
                                                                 { OptionsConfig::nbTabHt_Medium, "MEDIUM" },
                                                                 { OptionsConfig::nbTabHt_Short, "SHORT" },
                                                                 { OptionsConfig::nbTabHt_Tiny, "TINY" } };

    std::vector<wxString> tab_heights = { "TALL", "MEDIUM", "SHORT", "TINY" };

    AddProperty(_("Height"), tab_heights, tab_height_map_reverse.find(m_options->GetNotebookTabHeight())->second,
                [this, tab_height_map](const wxString& label, const wxAny& value) {
                    wxString value_str;
                    if (value.GetAs(&value_str)) {
                        m_options->SetNotebookTabHeight(tab_height_map.find(value_str)->second);
                    }
                });

    std::unordered_map<wxString, wxDirection> orientation_map = {
        { "UP", wxUP }, { "DOWN", wxDOWN }, { "LEFT", wxLEFT }, { "RIGHT", wxRIGHT }, { "VERTICAL", wxLEFT }
    };

    std::unordered_map<int, wxString> orientation_map_reverse = {
        { wxUP, "UP" }, { wxDOWN, "DOWN" }, { wxLEFT, "LEFT" }, { wxRIGHT, "RIGHT" }, { wxLEFT, "VERTICAL" }
    };

    std::vector<wxString> sidebar_tab_orientation = { "VERTICAL", "UP", "DOWN" };
    AddProperty(_("Sidebar buttons position"), sidebar_tab_orientation,
                orientation_map_reverse[m_options->GetWorkspaceTabsDirection()],
                [this, orientation_map](const wxString&, const wxAny& value) mutable {
                    wxString str_value;
                    if (value.GetAs(&str_value)) {
                        m_options->SetWorkspaceTabsDirection(orientation_map[str_value]);
                    }
                });
    AddProperty(_("Output tabs orientation"), std::vector<wxString>{ "UP", "DOWN" },
                orientation_map_reverse[m_options->GetOutputTabsDirection()],
                [this, orientation_map](const wxString&, const wxAny& value) mutable {
                    wxString str_value;
                    if (value.GetAs(&str_value)) {
                        m_options->SetOutputTabsDirection(orientation_map[str_value]);
                    }
                });

    AddProperty(_("Show close button on tabs"), m_options->IsTabHasXButton(), UPDATE_BOOL_CB(SetTabHasXButton));
    AddProperty(_("Show file path on tab label"), m_options->IsTabShowPath(), UPDATE_BOOL_CB(SetTabShowPath));

#if !MAINBOOK_AUIBOOK
    AddProperty(_("Mouse scroll switch bewtween tabs"), m_options->IsMouseScrollSwitchTabs(),
                UPDATE_BOOL_CB(SetMouseScrollSwitchTabs));
    AddProperty(_("Sort tab file list"), m_options->IsSortTabsDropdownAlphabetically(),
                UPDATE_BOOL_CB(SetSortTabsDropdownAlphabetically));
#endif

    AddProperty(_("Use Ctrl+TAB to switch tabs"), m_options->IsCtrlTabEnabled(), UPDATE_BOOL_CB(SetCtrlTabEnabled));
#ifndef __WXGTK__
    AddProperty(_("Hide main tab bar"), clConfig::Get().Read("HideTabBar", false),
                UPDATE_CLCONFIG_BOOL_CB("HideTabBar"));
#endif
    AddHeader(_("Find in files"));
    AddProperty(_("Don't automatically fold search results"), m_options->GetDontAutoFoldResults(),
                UPDATE_BOOL_CB(SetDontAutoFoldResults));
    AddProperty(_("When searching, don't override search string with current selection"),
                m_options->GetDontOverrideSearchStringWithSelection(),
                UPDATE_BOOL_CB(SetDontOverrideSearchStringWithSelection));
    AddHeader(_("Find next/Find previous"));
    AddProperty(_("Find next/Find previous actions override search string with current selection (like Find next at caret/prev at caret)"),
                m_options->GetFindNextOrPreviousUseSelection(),
                UPDATE_BOOL_CB(SetFindNextOrPreviousUseSelection));
}
