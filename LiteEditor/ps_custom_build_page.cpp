//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : ps_custom_build_page.cpp
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

#include "ps_custom_build_page.h"
#include "project_settings_dlg.h"
#include "dirsaver.h"
#include "project.h"
#include "globals.h"
#include "manager.h"
#include <wx/log.h>
#include "ProjectCustomBuildTragetDlg.h"

PSCustomBuildPage::PSCustomBuildPage( wxWindow* parent, const wxString &projectName, ProjectSettingsDlg *dlg  )
    : PSCustomBuildBasePage( parent )
    , m_projectName(projectName)
    , m_selecteCustomTaregt(wxNOT_FOUND)
    , m_dlg(dlg)
{
    m_listCtrlTargets->InsertColumn(0, _("Target"));
    m_listCtrlTargets->InsertColumn(1, _("Command"));
}

void PSCustomBuildPage::OnCustomBuildEnabled( wxCommandEvent& event )
{
    GetDlg()->SetCustomBuildEnabled(event.IsChecked());
}

void PSCustomBuildPage::OnCustomBuildEnabledUI( wxUpdateUIEvent& event )
{
    event.Enable(GetDlg()->IsCustomBuildEnabled());
}

void PSCustomBuildPage::OnCmdEvtVModified( wxCommandEvent& event )
{
    GetDlg()->SetIsDirty(true);
}

void PSCustomBuildPage::OnBrowseCustomBuildWD( wxCommandEvent& event )
{
    DirSaver ds;

    // Since all paths are relative to the project, set the working directory to the
    // current project path
    ProjectPtr p = ManagerST::Get()->GetProject(m_projectName);
    if (p) {
        wxSetWorkingDirectory(p->GetFileName().GetPath());
    }

    wxFileName fn(m_textCtrlCustomBuildWD->GetValue());
    wxString initPath(wxEmptyString);

    if (fn.DirExists()) {
        fn.MakeAbsolute();
        initPath = fn.GetFullPath();
    }

    wxString new_path = wxDirSelector(_("Select working directory:"), initPath, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if (new_path.IsEmpty() == false) {
        m_textCtrlCustomBuildWD->SetValue(new_path);
    }
}

void PSCustomBuildPage::OnItemActivated( wxListEvent& event )
{
    m_selecteCustomTaregt = event.m_itemIndex;
    DoEditTarget(m_selecteCustomTaregt);
}

void PSCustomBuildPage::OnItemSelected( wxListEvent& event )
{
    m_selecteCustomTaregt = event.m_itemIndex;
}

void PSCustomBuildPage::OnNewTarget( wxCommandEvent& event )
{
    wxUnusedVar(event);

    ProjectCustomBuildTragetDlg dlg(this, "", "");
    if (dlg.ShowModal() == wxID_OK) {
        GetDlg()->SetIsDirty(true);
        if (GetTargetCommand(dlg.GetName()).IsEmpty() == false) {
            wxMessageBox(wxString::Format(_("Target '%s' already exist!"), dlg.GetName().c_str()), _("CodeLite"), wxICON_WARNING|wxCENTER|wxOK, this);
            return;
        }
        long item = AppendListCtrlRow(m_listCtrlTargets);
        DoUpdateTarget(item, dlg.GetTargetName(), dlg.GetTargetCommand());
    }
}

void PSCustomBuildPage::OnEditTarget( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_selecteCustomTaregt != wxNOT_FOUND) {
        DoEditTarget(m_selecteCustomTaregt);
    }
}

void PSCustomBuildPage::OnEditTargetUI( wxUpdateUIEvent& event )
{
    event.Enable(m_selecteCustomTaregt != wxNOT_FOUND && m_checkEnableCustomBuild->IsChecked());
}

void PSCustomBuildPage::OnDeleteTarget( wxCommandEvent& event )
{
    wxUnusedVar(event);
    if (m_selecteCustomTaregt != wxNOT_FOUND) {
        m_listCtrlTargets->DeleteItem(m_selecteCustomTaregt);
        m_selecteCustomTaregt = wxNOT_FOUND;

        GetDlg()->SetIsDirty(true);
    }
}

void PSCustomBuildPage::OnDeleteTargetUI( wxUpdateUIEvent& event )
{
    if (m_selecteCustomTaregt != wxNOT_FOUND) {
        wxString name = GetColumnText(m_listCtrlTargets, m_selecteCustomTaregt, 0);
        event.Enable(name != ProjectCustomBuildTragetDlg::CUSTOM_TARGET_BUILD               &&
                     name != ProjectCustomBuildTragetDlg::CUSTOM_TARGET_CLEAN               &&
                     name != ProjectCustomBuildTragetDlg::CUSTOM_TARGET_REBUILD             &&
                     name != ProjectCustomBuildTragetDlg::CUSTOM_TARGET_COMPILE_SINGLE_FILE &&
                     m_checkEnableCustomBuild->IsChecked());
    } else {
        event.Enable(false);
    }
}

void PSCustomBuildPage::DoEditTarget(long item)
{
    if (item != wxNOT_FOUND) {
        wxString target = GetColumnText(m_listCtrlTargets, item, 0);
        wxString cmd    = GetColumnText(m_listCtrlTargets, item, 1);
        ProjectCustomBuildTragetDlg dlg(this, target, cmd);
        if (dlg.ShowModal() == wxID_OK) {
            DoUpdateTarget(item, dlg.GetTargetName(), dlg.GetTargetCommand());
            GetDlg()->SetIsDirty(true);
        }

#ifdef __WXMAC__
        GetDlg()->Raise();
#endif

    }
}

void PSCustomBuildPage::DoUpdateTarget(long item, const wxString& target, const wxString& cmd)
{
    SetColumnText(m_listCtrlTargets, item, 0, target);
    SetColumnText(m_listCtrlTargets, item, 1, cmd);

    m_listCtrlTargets->SetColumnWidth(0, 150);
    m_listCtrlTargets->SetColumnWidth(1, 300);
}

void PSCustomBuildPage::OnCustomBuildCBEnabledUI(wxUpdateUIEvent& event)
{
    GetDlg()->SetCustomBuildEnabled( m_checkEnableCustomBuild->IsChecked() );
}

ProjectSettingsDlg* PSCustomBuildPage::GetDlg()
{
    return m_dlg;
}

wxString PSCustomBuildPage::GetTargetCommand(const wxString& target)
{
    for (int i=0; i<(int)m_listCtrlTargets->GetItemCount(); i++) {
        if (GetColumnText(m_listCtrlTargets, i, 0) == target) {
            return GetColumnText(m_listCtrlTargets, i, 1);
        }
    }
    return wxEmptyString;
}

void PSCustomBuildPage::Load(BuildConfigPtr buildConf)
{
    m_checkEnableCustomBuild->SetValue(buildConf->IsCustomBuild());
    m_textCtrlCustomBuildWD->SetValue(buildConf->GetCustomBuildWorkingDir());

    m_listCtrlTargets->DeleteAllItems();
    long item = AppendListCtrlRow(m_listCtrlTargets);
    SetColumnText(m_listCtrlTargets, item, 0, ProjectCustomBuildTragetDlg::CUSTOM_TARGET_BUILD);
    SetColumnText(m_listCtrlTargets, item, 1, buildConf->GetCustomBuildCmd());

    item = AppendListCtrlRow(m_listCtrlTargets);
    SetColumnText(m_listCtrlTargets, item, 0, ProjectCustomBuildTragetDlg::CUSTOM_TARGET_CLEAN);
    SetColumnText(m_listCtrlTargets, item, 1, buildConf->GetCustomCleanCmd());

    item = AppendListCtrlRow(m_listCtrlTargets);
    SetColumnText(m_listCtrlTargets, item, 0, ProjectCustomBuildTragetDlg::CUSTOM_TARGET_REBUILD);
    SetColumnText(m_listCtrlTargets, item, 1, buildConf->GetCustomRebuildCmd());

    item = AppendListCtrlRow(m_listCtrlTargets);
    SetColumnText(m_listCtrlTargets, item, 0, ProjectCustomBuildTragetDlg::CUSTOM_TARGET_COMPILE_SINGLE_FILE);
    SetColumnText(m_listCtrlTargets, item, 1, buildConf->GetSingleFileBuildCommand());

    item = AppendListCtrlRow(m_listCtrlTargets);
    SetColumnText(m_listCtrlTargets, item, 0, ProjectCustomBuildTragetDlg::CUSTOM_TARGET_PREPROCESS_FILE);
    SetColumnText(m_listCtrlTargets, item, 1, buildConf->GetPreprocessFileCommand());

    // Initialize the custom build targets
    std::map<wxString, wxString> targets = buildConf->GetCustomTargets();
    std::map<wxString, wxString>::iterator titer = targets.begin();
    for (; titer != targets.end(); titer++) {

        if ( ProjectCustomBuildTragetDlg::IsPredefinedTarget( titer->first ) )
            continue;

        item = AppendListCtrlRow(m_listCtrlTargets);
        SetColumnText(m_listCtrlTargets, item, 0, titer->first);
        SetColumnText(m_listCtrlTargets, item, 1, titer->second);
    }

    m_listCtrlTargets->SetColumnWidth(0, 150);
    m_listCtrlTargets->SetColumnWidth(1, 300);

    m_dlg->SetCustomBuildEnabled( m_checkEnableCustomBuild->IsChecked() );
}

void PSCustomBuildPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    // loop over the list and create the targets map
    std::map<wxString, wxString> targets;
    for (int i=0; i<(int)m_listCtrlTargets->GetItemCount(); i++) {
        wxString colText = GetColumnText(m_listCtrlTargets, i, 0);
        if( ProjectCustomBuildTragetDlg::IsPredefinedTarget( colText ) )
            continue;

        targets[GetColumnText(m_listCtrlTargets, i, 0)] = GetColumnText(m_listCtrlTargets, i, 1);
    }

    buildConf->SetCustomTargets(targets);
    buildConf->SetCustomBuildCmd(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_BUILD));
    buildConf->SetCustomCleanCmd(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_CLEAN));
    buildConf->SetCustomRebuildCmd(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_REBUILD));
    buildConf->SetSingleFileBuildCommand(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_COMPILE_SINGLE_FILE));
    buildConf->SetPreprocessFileCommand(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_PREPROCESS_FILE));
    buildConf->EnableCustomBuild(m_checkEnableCustomBuild->IsChecked());
    buildConf->SetCustomBuildWorkingDir(m_textCtrlCustomBuildWD->GetValue());
}

void PSCustomBuildPage::Clear()
{
    m_checkEnableCustomBuild->SetValue(false);
    m_listCtrlTargets->DeleteAllItems();
    m_textCtrlCustomBuildWD->ChangeValue(wxT("$(WorkspacePath)"));
}
void PSCustomBuildPage::OnProjectEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dlg->IsProjectEnabled());
}
