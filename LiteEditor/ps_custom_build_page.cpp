//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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

#include "ProjectCustomBuildTragetDlg.h"
#include "dirsaver.h"
#include "globals.h"
#include "macros.h"
#include "manager.h"
#include "project.h"
#include "project_settings_dlg.h"

#include <wx/dirdlg.h>
#include <wx/log.h>

PSCustomBuildPage::PSCustomBuildPage(wxWindow* parent, const wxString& projectName, ProjectSettingsDlg* dlg)
    : PSCustomBuildBasePage(parent)
    , m_projectName(projectName)
    , m_dlg(dlg)
{
}

void PSCustomBuildPage::OnCustomBuildEnabled(wxCommandEvent& event)
{
    GetDlg()->SetCustomBuildEnabled(event.IsChecked());
}

void PSCustomBuildPage::OnCustomBuildEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable(GetDlg()->IsCustomBuildEnabled());
}

void PSCustomBuildPage::OnCmdEvtVModified(wxCommandEvent& event) { GetDlg()->SetIsDirty(true); }

void PSCustomBuildPage::OnBrowseCustomBuildWD(wxCommandEvent& event)
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

    wxString new_path =
        wxDirSelector(_("Select working directory:"), initPath, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if (new_path.IsEmpty() == false) {
        m_textCtrlCustomBuildWD->SetValue(new_path);
    }
}

void PSCustomBuildPage::OnNewTarget(wxCommandEvent& event)
{
    wxUnusedVar(event);

    ProjectCustomBuildTragetDlg dlg(this, "", "");
    if (dlg.ShowModal() == wxID_OK) {
        GetDlg()->SetIsDirty(true);
        if (GetTargetCommand(dlg.GetName()).IsEmpty() == false) {
            wxMessageBox(wxString::Format(_("Target '%s' already exist!"), dlg.GetName().c_str()), _("CodeLite"),
                         wxICON_WARNING | wxCENTER | wxOK, this);
            return;
        }
        wxVector<wxVariant> cols;
        cols.push_back(dlg.GetTargetName());
        cols.push_back(dlg.GetTargetCommand());
        m_dvListCtrlTargets->AppendItem(cols);
    }
}

void PSCustomBuildPage::OnEditTarget(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    CHECK_ITEM_RET(item);
    DoEditTarget(item);
}

void PSCustomBuildPage::OnEditTargetUI(wxUpdateUIEvent& event)
{
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    event.Enable(item.IsOk() && m_checkEnableCustomBuild->IsChecked());
}

void PSCustomBuildPage::OnDeleteTarget(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    if (item.IsOk()) {
        m_dvListCtrlTargets->DeleteItem(m_dvListCtrlTargets->ItemToRow(item));
        GetDlg()->SetIsDirty(true);
    }
}

void PSCustomBuildPage::OnDeleteTargetUI(wxUpdateUIEvent& event)
{
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    if (item.IsOk()) {
        wxVariant vTarget;
        m_dvListCtrlTargets->GetValue(vTarget, m_dvListCtrlTargets->ItemToRow(item), 0);
        wxString name = vTarget.GetString();
        event.Enable(name != ProjectCustomBuildTragetDlg::CUSTOM_TARGET_BUILD &&
                     name != ProjectCustomBuildTragetDlg::CUSTOM_TARGET_CLEAN &&
                     name != ProjectCustomBuildTragetDlg::CUSTOM_TARGET_REBUILD &&
                     name != ProjectCustomBuildTragetDlg::CUSTOM_TARGET_COMPILE_SINGLE_FILE &&
                     m_checkEnableCustomBuild->IsChecked());
    } else {
        event.Enable(false);
    }
}

void PSCustomBuildPage::DoEditTarget(wxDataViewItem item)
{
    CHECK_ITEM_RET(item);
    wxVariant varname, varcommand;
    m_dvListCtrlTargets->GetValue(varname, m_dvListCtrlTargets->ItemToRow(item), 0);
    m_dvListCtrlTargets->GetValue(varcommand, m_dvListCtrlTargets->ItemToRow(item), 1);
    wxString target = varname.GetString();
    wxString cmd = varcommand.GetString();
    ProjectCustomBuildTragetDlg dlg(this, target, cmd);
    if (dlg.ShowModal() == wxID_OK) {
        DoUpdateTarget(item, dlg.GetTargetName(), dlg.GetTargetCommand());
        GetDlg()->SetIsDirty(true);
    }

#ifdef __WXMAC__
    GetDlg()->Raise();
#endif
}

void PSCustomBuildPage::DoUpdateTarget(wxDataViewItem item, const wxString& target, const wxString& cmd)
{
    CHECK_ITEM_RET(item);
    int row = m_dvListCtrlTargets->ItemToRow(item);

    m_dvListCtrlTargets->SetValue(target, row, 0);
    m_dvListCtrlTargets->SetValue(cmd, row, 1);
}

void PSCustomBuildPage::OnCustomBuildCBEnabledUI(wxUpdateUIEvent& event)
{
    GetDlg()->SetCustomBuildEnabled(m_checkEnableCustomBuild->IsChecked());
}

ProjectSettingsDlg* PSCustomBuildPage::GetDlg() { return m_dlg; }

wxString PSCustomBuildPage::GetTargetCommand(const wxString& target)
{
    for (int i = 0; i < m_dvListCtrlTargets->GetItemCount(); i++) {
        wxVariant variantTarget;
        m_dvListCtrlTargets->GetValue(variantTarget, i, 0);
        if (variantTarget.GetString() == target) {
            wxVariant command;
            m_dvListCtrlTargets->GetValue(command, i, 1);
            return command.GetString();
        }
    }
    return wxEmptyString;
}

void PSCustomBuildPage::Load(BuildConfigPtr buildConf)
{
    m_checkEnableCustomBuild->SetValue(buildConf->IsCustomBuild());
    m_textCtrlCustomBuildWD->SetValue(buildConf->GetCustomBuildWorkingDir());

    m_dvListCtrlTargets->DeleteAllItems();

    wxVector<wxVariant> cols;

    cols.clear();
    cols.push_back(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_BUILD);
    cols.push_back(buildConf->GetCustomBuildCmd());
    m_dvListCtrlTargets->AppendItem(cols);

    cols.clear();
    cols.push_back(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_CLEAN);
    cols.push_back(buildConf->GetCustomCleanCmd());
    m_dvListCtrlTargets->AppendItem(cols);

    cols.clear();
    cols.push_back(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_REBUILD);
    cols.push_back(buildConf->GetCustomRebuildCmd());
    m_dvListCtrlTargets->AppendItem(cols);

    cols.clear();
    cols.push_back(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_COMPILE_SINGLE_FILE);
    cols.push_back(buildConf->GetSingleFileBuildCommand());
    m_dvListCtrlTargets->AppendItem(cols);

    cols.clear();
    cols.push_back(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_PREPROCESS_FILE);
    cols.push_back(buildConf->GetPreprocessFileCommand());
    m_dvListCtrlTargets->AppendItem(cols);

    // Initialize the custom build targets
    std::map<wxString, wxString> targets = buildConf->GetCustomTargets();
    std::map<wxString, wxString>::iterator titer = targets.begin();
    for (; titer != targets.end(); ++titer) {

        if (ProjectCustomBuildTragetDlg::IsPredefinedTarget(titer->first))
            continue;

        cols.clear();
        cols.push_back(titer->first);
        cols.push_back(titer->second);
        m_dvListCtrlTargets->AppendItem(cols);
    }
    m_dlg->SetCustomBuildEnabled(m_checkEnableCustomBuild->IsChecked());
}

void PSCustomBuildPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    // loop over the list and create the targets map
    std::map<wxString, wxString> targets;
    for (int i = 0; i < m_dvListCtrlTargets->GetItemCount(); i++) {
        wxVariant vTarget, vCommand;
        m_dvListCtrlTargets->GetValue(vTarget, i, 0);
        m_dvListCtrlTargets->GetValue(vCommand, i, 1);
        if (ProjectCustomBuildTragetDlg::IsPredefinedTarget(vTarget.GetString()))
            continue;

        targets[vTarget.GetString()] = vCommand.GetString();
    }

    buildConf->SetCustomTargets(targets);
    buildConf->SetCustomBuildCmd(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_BUILD));
    buildConf->SetCustomCleanCmd(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_CLEAN));
    buildConf->SetCustomRebuildCmd(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_REBUILD));
    buildConf->SetSingleFileBuildCommand(
        GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_COMPILE_SINGLE_FILE));
    buildConf->SetPreprocessFileCommand(GetTargetCommand(ProjectCustomBuildTragetDlg::CUSTOM_TARGET_PREPROCESS_FILE));
    buildConf->EnableCustomBuild(m_checkEnableCustomBuild->IsChecked());
    buildConf->SetCustomBuildWorkingDir(m_textCtrlCustomBuildWD->GetValue());
}

void PSCustomBuildPage::Clear()
{
    m_checkEnableCustomBuild->SetValue(false);
    m_dvListCtrlTargets->DeleteAllItems();
    m_textCtrlCustomBuildWD->ChangeValue(wxT("$(WorkspacePath)"));
}
void PSCustomBuildPage::OnProjectEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_dlg->IsProjectEnabled()); }

void PSCustomBuildPage::OnTargetActivated(wxDataViewEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent dummy;
    OnEditTarget(dummy);
}
void PSCustomBuildPage::OnEnableTableUI(wxUpdateUIEvent& event) { event.Enable(m_checkEnableCustomBuild->IsChecked()); }
