//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : batchbuilddlg.cpp
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

#include <wx/tokenzr.h>
#include "globals.h"
#include "manager.h"
#include "workspace.h"
#include "batchbuilddlg.h"

BatchBuildDlg::BatchBuildDlg( wxWindow* parent )
    : BatchBuildBaseDlg( parent )
{
    m_checkListConfigurations->SetFocus();
    DoInitialize();
}

void BatchBuildDlg::OnItemSelected( wxCommandEvent& event )
{
    event.Skip();
}

void BatchBuildDlg::OnItemToggled( wxCommandEvent& event )
{
    wxUnusedVar(event);
}

void BatchBuildDlg::OnBuild( wxCommandEvent& event )
{
    wxUnusedVar(event);
    m_cmd = QueueCommand::kBuild;
    DoSaveBatchBuildOrder();
    EndModal(wxID_OK);
}

void BatchBuildDlg::OnBuildUI( wxUpdateUIEvent& event )
{
    bool enable(false);
    for (unsigned int i=0; i<m_checkListConfigurations->GetCount(); i++) {
        if (m_checkListConfigurations->IsChecked(i)) {
            enable = true;
            break;
        }
    }

    event.Enable(enable);
}

void BatchBuildDlg::OnClean( wxCommandEvent& event )
{
    wxUnusedVar(event);
    m_cmd = QueueCommand::kClean;
    DoSaveBatchBuildOrder();
    EndModal(wxID_OK);
}

void BatchBuildDlg::OnCleanUI( wxUpdateUIEvent& event )
{
    bool enable(false);
    for (unsigned int i=0; i<m_checkListConfigurations->GetCount(); i++) {
        if (m_checkListConfigurations->IsChecked(i)) {
            enable = true;
            break;
        }
    }

    event.Enable(enable);
}

void BatchBuildDlg::OnCheckAll( wxCommandEvent& event )
{
    wxUnusedVar(event);
    for (unsigned int i=0; i<m_checkListConfigurations->GetCount(); i++) {
        m_checkListConfigurations->Check(i, true);
    }
}

void BatchBuildDlg::OnUnCheckAll( wxCommandEvent& event )
{
    wxUnusedVar(event);
    for (unsigned int i=0; i<m_checkListConfigurations->GetCount(); i++) {
        m_checkListConfigurations->Check(i, false);
    }
}

void BatchBuildDlg::OnMoveUp( wxCommandEvent& event )
{
    wxString selectedString  = m_checkListConfigurations->GetStringSelection();

    int sel = m_checkListConfigurations->GetSelection();
    if (sel == wxNOT_FOUND) {
        return;
    }
    bool checked = m_checkListConfigurations->IsChecked(sel);
    sel --;
    if (sel < 0) {
        return;
    }

    // sel contains the new position we want to place the selection string
    m_checkListConfigurations->Delete(sel + 1);
    m_checkListConfigurations->Insert(selectedString, sel);
    m_checkListConfigurations->Check(sel, checked);
    m_checkListConfigurations->Select(sel);
}

void BatchBuildDlg::OnMoveUpUI( wxUpdateUIEvent& event )
{
    event.Skip();
}

void BatchBuildDlg::OnMoveDown( wxCommandEvent& event )
{
    int sel = m_checkListConfigurations->GetSelection();
    if (sel == wxNOT_FOUND) {
        return;
    }
    bool checked = m_checkListConfigurations->IsChecked(sel);
    sel ++;
    if (sel >= (int)m_checkListConfigurations->GetCount()) {
        return;
    }

    // sel contains the new position we want to place the selection string
    wxString oldStr = m_checkListConfigurations->GetString(sel);

    m_checkListConfigurations->Delete(sel);
    m_checkListConfigurations->Insert(oldStr, sel - 1);
    m_checkListConfigurations->Check(sel -1, checked);
    m_checkListConfigurations->Select(sel);
}

void BatchBuildDlg::OnMoveDownUI( wxUpdateUIEvent& event )
{
    event.Skip();
}

void BatchBuildDlg::OnClose( wxCommandEvent& event )
{
    wxUnusedVar(event);
    DoSaveBatchBuildOrder();
    EndModal(wxID_CANCEL);
}

std::list<QueueCommand> BatchBuildDlg::GetBuildInfoList() const
{
    std::list<QueueCommand> buildInfoList;
    bool clean_log(true);
    for (unsigned int i=0; i<m_checkListConfigurations->GetCount(); i++) {
        if (m_checkListConfigurations->IsChecked(i)) {
            wxString text = m_checkListConfigurations->GetString(i);
            wxString project = text.BeforeFirst(wxT('|'));
            wxString config  = text.AfterFirst(wxT('|'));

            project.Trim().Trim(false);
            config.Trim().Trim(false);

            // get the selected configuration to be built
            BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, config);
            if (bldConf) {

                QueueCommand buildInfo(project, config, true, m_cmd);
                // handle custom build projects properly
                if (bldConf->IsCustomBuild()) {
                    buildInfo.SetKind(QueueCommand::kCustomBuild);
                    switch (m_cmd) {
                    case QueueCommand::kBuild:
                        buildInfo.SetCustomBuildTarget(wxT("Build"));
                        break;
                    case QueueCommand::kClean:
                        buildInfo.SetCustomBuildTarget(wxT("Clean"));
                        break;
                    }
                }
                buildInfo.SetCleanLog(clean_log);
                buildInfoList.push_back(buildInfo);
                clean_log = false;
            }
        }
    }
    return buildInfoList;
}

void BatchBuildDlg::DoInitialize()
{
    // load the previously saved batch build file
    wxFileName fn(clCxxWorkspaceST::Get()->GetWorkspaceFileName());
    fn.SetExt(wxT("batch_build"));

    wxString content;
    wxArrayString arr;
    if (ReadFileWithConversion(fn.GetFullPath(), content)) {
        arr = wxStringTokenize(content, wxT("\n"), wxTOKEN_STRTOK);
        for (size_t i=0; i<arr.GetCount(); i++) {
            int idx = m_checkListConfigurations->Append(arr.Item(i));
            m_checkListConfigurations->Check((unsigned int)idx);
        }
    }

    // loop over all projects, for each project collect all available
    // build configurations and add them to the check list control
    wxArrayString projects;
    clCxxWorkspaceST::Get()->GetProjectList(projects);
    for (size_t i=0; i<projects.GetCount(); i++) {
        ProjectPtr p = ManagerST::Get()->GetProject(projects.Item(i));
        if (p) {
            ProjectSettingsPtr settings = p->GetSettings();
            if (settings) {
                ProjectSettingsCookie cookie;
                BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
                while (bldConf) {
                    wxString item(p->GetName() + wxT(" | ") + bldConf->GetName());

                    int where = arr.Index(item);
                    if (where == wxNOT_FOUND) {
                        // append this item
                        m_checkListConfigurations->Append(item);
                    } else {
                        // this item already been added,
                        // remove it from the arr and continue
                        arr.RemoveAt((size_t)where);
                    }

                    bldConf = settings->GetNextBuildConfiguration(cookie);
                }
            }
        }
    }

    // check to see which configuration was left in 'arr'
    // and remove them from the checklistbox
    for (size_t i=0; i<arr.GetCount(); i++) {
        int where = m_checkListConfigurations->FindString(arr.Item(i));
        if (where != wxNOT_FOUND) {
            m_checkListConfigurations->Delete((unsigned int)where);
        }
    }
    arr.clear();

    if (m_checkListConfigurations->GetCount()>0) {
        m_checkListConfigurations->Select(0);
    }
}

void BatchBuildDlg::DoSaveBatchBuildOrder()
{
    wxFileName fn(clCxxWorkspaceST::Get()->GetWorkspaceFileName());
    fn.SetExt(wxT("batch_build"));

    wxString content;
    for (unsigned int i=0; i<m_checkListConfigurations->GetCount(); i++) {
        if (m_checkListConfigurations->IsChecked(i)) {
            content << m_checkListConfigurations->GetString(i) << wxT("\n");
        }
    }

    WriteFileWithBackup(fn.GetFullPath(), content, false);
}
