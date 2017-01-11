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
#include "editor_config.h"
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
    m_checkBoxEditorTabsFollowsTheme->SetValue(options->IsTabColourMatchesTheme());
    m_checkBoxUseDarkTabTheme->SetValue(options->IsTabColourDark());
    m_checkBoxShowXButton->SetValue(options->IsTabHasXButton());
    m_choiceTabStyle->SetSelection(options->GetOptions() & OptionsConfig::Opt_TabStyleMinimal ? 1 : 0);
    int sel(0);
    switch(options->GetNotebookTabHeight()) {
        case OptionsConfig::nbTabHt_Tiny: sel = 3; break;
        case OptionsConfig::nbTabHt_Short: sel = 2; break;
        case OptionsConfig::nbTabHt_Medium: sel = 1; break;
        default: sel = 0;
    }
    m_choiceTabHeight->SetSelection(sel);
#if 0
    {
        wxArrayString tabOptionsArr;
        tabOptionsArr.Add(wxT("TOP"));
        tabOptionsArr.Add(wxT("BOTTOM"));
        m_choiceWorkspaceTabsOrientation->Clear();
        m_choiceWorkspaceTabsOrientation->Append(tabOptionsArr);
    }
#endif
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

#if 0
    // On OSX we dont support left-right (due to blurred images)
    switch(options->GetWorkspaceTabsDirection()) {
    case wxLEFT:
    case wxTOP:
        m_choiceWorkspaceTabsOrientation->SetSelection(0);
        break;
    case wxRIGHT:
    case wxBOTTOM:
        m_choiceWorkspaceTabsOrientation->SetSelection(1);
        break;
    default:
        break;
    }
#else
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

    m_checkBoxHideOutputPaneNotIfDebug->Connect(wxEVT_UPDATE_UI,
        wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL, this);
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
    options->SetTabColourMatchesTheme(m_checkBoxEditorTabsFollowsTheme->IsChecked());
    options->SetTabColourDark(m_checkBoxUseDarkTabTheme->IsChecked());
    options->SetTabHasXButton(m_checkBoxShowXButton->IsChecked());
    options->EnableOption(OptionsConfig::Opt_TabStyleMinimal, (m_choiceTabStyle->GetSelection() == 1));
    int ht(0);
    switch(m_choiceTabHeight->GetSelection()) {
        case 3: ht = OptionsConfig::nbTabHt_Tiny; break;
        case 2: ht = OptionsConfig::nbTabHt_Short; break;
        case 1: ht = OptionsConfig::nbTabHt_Medium; break;
        default: ht = OptionsConfig::nbTabHt_Tall;
    }
    options->SetNotebookTabHeight(ht);
    
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
