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

#include "cl_defs.h"
#include "editor_config.h"
#include "editorsettingsdockingwidows.h"
#include "frame.h"

EditorSettingsDockingWindows::EditorSettingsDockingWindows(wxWindow* parent)
    : EditorSettingsDockingWindowsBase(parent)
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();

    m_checkBoxHideOutputPaneOnClick->SetValue(options->GetHideOutpuPaneOnUserClick());
    m_checkBoxHideOutputPaneNotIfBuild->SetValue(options->GetHideOutputPaneNotIfBuild());
    m_checkBoxHideOutputPaneNotIfSearch->SetValue(options->GetHideOutputPaneNotIfSearch());
    m_checkBoxHideOutputPaneNotIfReplace->SetValue(options->GetHideOutputPaneNotIfReplace());
    m_checkBoxHideOutputPaneNotIfReferences->SetValue(options->GetHideOutputPaneNotIfReferences());
    m_checkBoxHideOutputPaneNotIfOutput->SetValue(options->GetHideOutputPaneNotIfOutput());
    m_checkBoxHideOutputPaneNotIfTrace->SetValue(options->GetHideOutputPaneNotIfTrace());
    m_checkBoxHideOutputPaneNotIfTasks->SetValue(options->GetHideOutputPaneNotIfTasks());
    m_checkBoxHideOutputPaneNotIfBuildQ->SetValue(options->GetHideOutputPaneNotIfBuildQ());
    m_checkBoxHideOutputPaneNotIfCppCheck->SetValue(options->GetHideOutputPaneNotIfCppCheck());
    m_checkBoxHideOutputPaneNotIfSvn->SetValue(options->GetHideOutputPaneNotIfSvn());
    m_checkBoxHideOutputPaneNotIfCscope->SetValue(options->GetHideOutputPaneNotIfCscope());
    m_checkBoxHideOutputPaneNotIfGit->SetValue(options->GetHideOutputPaneNotIfGit());
    m_checkBoxHideOutputPaneNotIfDebug->SetValue(options->GetHideOutputPaneNotIfDebug());
    m_checkBoxHideOutputPaneNotIfMemCheck->SetValue(options->GetHideOutputPaneNotIfMemCheck());
    m_checkBoxFindBarAtBottom->SetValue(options->GetFindBarAtBottom());
    m_checkBoxDontFoldSearchResults->SetValue(options->GetDontAutoFoldResults());
    m_checkBoxShowDebugOnRun->SetValue(options->GetShowDebugOnRun());
    m_radioBoxHint->SetSelection(options->GetDockingStyle());
    m_checkBoxHideCaptions->SetValue(!options->IsShowDockingWindowCaption());
    m_checkBoxEnsureCaptionsVisible->SetValue(options->IsEnsureCaptionsVisible());

    m_checkBoxShowXButton->SetValue(options->IsTabHasXButton());

    // DEFAULT 0
    // MINIMAL 1
    // TRAPEZOID 2
    if(options->GetOptions() & OptionsConfig::Opt_TabStyleTRAPEZOID) {
        m_choiceTabStyle->SetSelection(2);
    } else if(options->GetOptions() & OptionsConfig::Opt_TabStyleMinimal) {
        m_choiceTabStyle->SetSelection(1);
    } else {
        // default
        m_choiceTabStyle->SetSelection(0);
    }

#if !USE_AUI_NOTEBOOK
    m_checkBoxEditorTabsFollowsTheme->SetValue(options->IsTabColourMatchesTheme());
    m_checkBoxUseDarkTabTheme->SetValue(options->IsTabColourDark());
    m_checkBoxMouseScrollSwitchTabs->SetValue(options->IsMouseScrollSwitchTabs());
#else
    m_checkBoxEditorTabsFollowsTheme->SetValue(true);
    m_checkBoxEditorTabsFollowsTheme->Enable(false);
    m_checkBoxUseDarkTabTheme->SetValue(false);
    m_checkBoxUseDarkTabTheme->Enable(false);
    m_checkBoxMouseScrollSwitchTabs->SetValue(false);
    m_checkBoxMouseScrollSwitchTabs->Enable(false);
#endif

    int sel(0);
    switch(options->GetNotebookTabHeight()) {
    case OptionsConfig::nbTabHt_Tiny:
        sel = 3;
        break;
    case OptionsConfig::nbTabHt_Short:
        sel = 2;
        break;
    case OptionsConfig::nbTabHt_Medium:
        sel = 1;
        break;
    default:
        sel = 0;
    }
    m_choiceTabHeight->SetSelection(sel);

#if USE_AUI_NOTEBOOK
    m_choiceOutputTabsOrientation->SetSelection(0);
    m_choiceWorkspaceTabsOrientation->SetSelection(2);
    m_choiceOutputTabsOrientation->Enable(false);
    m_choiceWorkspaceTabsOrientation->Enable(false);
#else
    switch(options->GetOutputTabsDirection()) {
    case wxTOP:
        m_choiceOutputTabsOrientation->SetSelection(0);
        break;
    case wxBOTTOM:
        m_choiceOutputTabsOrientation->SetSelection(1);
        break;
    default:
        break;
    }
    switch(options->GetWorkspaceTabsDirection()) {
    case wxLEFT:
        m_choiceWorkspaceTabsOrientation->SetSelection(0);
        break;
    case wxRIGHT:
        m_choiceWorkspaceTabsOrientation->SetSelection(1);
        break;
    case wxTOP:
        m_choiceWorkspaceTabsOrientation->SetSelection(2);
        break;
    case wxBOTTOM:
        m_choiceWorkspaceTabsOrientation->SetSelection(3);
        break;
    default:
        break;
    }
#endif

    m_checkBoxHideOutputPaneNotIfDebug->Connect(
        wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
        this);
}

void EditorSettingsDockingWindows::Save(OptionsConfigPtr options)
{
    options->SetHideOutpuPaneOnUserClick(m_checkBoxHideOutputPaneOnClick->IsChecked());
    options->SetHideOutputPaneNotIfBuild(m_checkBoxHideOutputPaneNotIfBuild->IsChecked());
    options->SetHideOutputPaneNotIfSearch(m_checkBoxHideOutputPaneNotIfSearch->IsChecked());
    options->SetHideOutputPaneNotIfReplace(m_checkBoxHideOutputPaneNotIfReplace->IsChecked());
    options->SetHideOutputPaneNotIfReferences(m_checkBoxHideOutputPaneNotIfReferences->IsChecked());
    options->SetHideOutputPaneNotIfOutput(m_checkBoxHideOutputPaneNotIfOutput->IsChecked());
    options->SetHideOutputPaneNotIfTrace(m_checkBoxHideOutputPaneNotIfTrace->IsChecked());
    options->SetHideOutputPaneNotIfTasks(m_checkBoxHideOutputPaneNotIfTasks->IsChecked());
    options->SetHideOutputPaneNotIfBuildQ(m_checkBoxHideOutputPaneNotIfBuildQ->IsChecked());
    options->SetHideOutputPaneNotIfCppCheck(m_checkBoxHideOutputPaneNotIfCppCheck->IsChecked());
    options->SetHideOutputPaneNotIfSvn(m_checkBoxHideOutputPaneNotIfSvn->IsChecked());
    options->SetHideOutputPaneNotIfCscope(m_checkBoxHideOutputPaneNotIfCscope->IsChecked());
    options->SetHideOutputPaneNotIfGit(m_checkBoxHideOutputPaneNotIfGit->IsChecked());
    options->SetHideOutputPaneNotIfDebug(m_checkBoxHideOutputPaneNotIfDebug->IsChecked());
    options->SetHideOutputPaneNotIfMemCheck(m_checkBoxHideOutputPaneNotIfMemCheck->IsChecked());
    options->SetFindBarAtBottom(m_checkBoxFindBarAtBottom->IsChecked());
    options->SetDontAutoFoldResults(m_checkBoxDontFoldSearchResults->IsChecked());
    options->SetShowDebugOnRun(m_checkBoxShowDebugOnRun->IsChecked());
    options->SetDockingStyle(m_radioBoxHint->GetSelection());
    options->SetShowDockingWindowCaption(!m_checkBoxHideCaptions->IsChecked());
    options->SetEnsureCaptionsVisible(m_checkBoxEnsureCaptionsVisible->IsChecked());

#if USE_AUI_NOTEBOOK
    options->SetTabColourMatchesTheme(true);
    options->SetTabColourDark(false);
#else
    options->SetTabColourMatchesTheme(m_checkBoxEditorTabsFollowsTheme->IsChecked());
    options->SetTabColourDark(m_checkBoxUseDarkTabTheme->IsChecked());
#endif
    options->SetTabHasXButton(m_checkBoxShowXButton->IsChecked());
    options->SetMouseScrollSwitchTabs(m_checkBoxMouseScrollSwitchTabs->IsChecked());

    int ht(0);
    switch(m_choiceTabHeight->GetSelection()) {
    case 3:
        ht = OptionsConfig::nbTabHt_Tiny;
        break;
    case 2:
        ht = OptionsConfig::nbTabHt_Short;
        break;
    case 1:
        ht = OptionsConfig::nbTabHt_Medium;
        break;
    default:
        ht = OptionsConfig::nbTabHt_Tall;
    }
    options->SetNotebookTabHeight(ht);
    int tabStyleSelection = m_choiceTabStyle->GetSelection();
    options->EnableOption(OptionsConfig::Opt_TabStyleMinimal, (tabStyleSelection == 1));
    options->EnableOption(OptionsConfig::Opt_TabStyleTRAPEZOID, (tabStyleSelection == 2));

    // Set the tab style:
    // DEFAULT 0
    // MINIMAL 1
    // TRAPEZOID 2
#if USE_AUI_NOTEBOOK
    options->SetOutputTabsDirection(wxTOP);
    options->SetWorkspaceTabsDirection(wxTOP);
#else

    switch(m_choiceOutputTabsOrientation->GetSelection()) {
    case 0:
        options->SetOutputTabsDirection(wxTOP);
        break;
    case 1:
        options->SetOutputTabsDirection(wxBOTTOM);
        break;
    default:
        break;
    }
    switch(m_choiceWorkspaceTabsOrientation->GetSelection()) {
    case 0:
        options->SetWorkspaceTabsDirection(wxLEFT);
        break;
    case 1:
        options->SetWorkspaceTabsDirection(wxRIGHT);
        break;
    case 2:
        options->SetWorkspaceTabsDirection(wxTOP);
        break;
    case 3:
        options->SetWorkspaceTabsDirection(wxBOTTOM);
        break;
    default:
        break;
    }
#endif
}

void EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxHideOutputPaneOnClick->IsChecked());
}

bool EditorSettingsDockingWindows::IsRestartRequired() { return false; }
void EditorSettingsDockingWindows::OnEnsureCaptionsVisibleUI(wxUpdateUIEvent& event)
{
#ifdef __WXMSW__
    event.Enable(m_checkBoxHideCaptions->IsChecked());
#else
    event.Enable(false);
    event.Check(false);
#endif
}
