//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : edit_workspace_conf_dlg.cpp
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

#include "edit_workspace_conf_dlg.h"
#include "macros.h"
#include "manager.h"

///////////////////////////////////////////////////////////////////////////

EditWorkspaceConfDlg::EditWorkspaceConfDlg(wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size,
                                           int style)
    : EditWorkspaceConfDlgBase(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);
    this->Layout();

    CustomInit();
}

void EditWorkspaceConfDlg::OnListBoxDClick(wxCommandEvent& event) { DoRename(event.GetString()); }

void EditWorkspaceConfDlg::CustomInit()
{
    // fill the list box
    FillList();

    ConnectButton(m_buttonDelete, EditWorkspaceConfDlg::OnDelete);
    ConnectButton(m_buttonRename, EditWorkspaceConfDlg::OnRename);
    ConnectListBoxDClick(m_wspConfList, EditWorkspaceConfDlg::OnRename);
}

void EditWorkspaceConfDlg::FillList()
{
    m_wspConfList->Clear();
    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();

    for (auto& configuration : matrix->GetConfigurations()) {
        m_wspConfList->Append(configuration->GetName());
        m_wspConfList->SetSelection(0);
    }
}

void EditWorkspaceConfDlg::OnDelete(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_wspConfList->GetCount() == 0) {
        return;
    }
    wxString delMe = m_wspConfList->GetStringSelection();
    if(delMe.IsEmpty()) {
        return;
    }

    // remove the requested workspace build configuration
    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
    wxString msg = wxString::Format(_("Remove workspace configuration '%s'?"), delMe);
    if (wxMessageBox(msg, wxT("CodeLite"), wxICON_QUESTION | wxYES_NO | wxCANCEL) == wxYES) {
        matrix->RemoveConfiguration(delMe);
        // apply changes
        ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);

        // refresh list
        FillList();
    }
}

void EditWorkspaceConfDlg::DoRename(const wxString& selItem)
{
    wxTextEntryDialog* dlg = new wxTextEntryDialog(this, _("New Configuration Name:"), _("Rename"), selItem);
    if(dlg->ShowModal() == wxID_OK) {
        wxString newName = dlg->GetValue();
        TrimString(newName);
        if(!newName.IsEmpty()) {
            newName.Replace(" ", "_"); // using spaces will break the build, replace them with hyphens
            BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
            WorkspaceConfigurationPtr conf = matrix->GetConfigurationByName(selItem);
            // rename the configuration
            conf->SetName(newName);
            matrix->SetConfiguration(conf);
            matrix->RemoveConfiguration(selItem);

            // apply changes
            ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);
            // refresh list
            FillList();
        }
    }
    dlg->Destroy();
}

void EditWorkspaceConfDlg::OnRename(wxCommandEvent& event)
{
    wxString changeMe;
    wxUnusedVar(event);
    if(m_wspConfList->GetCount() == 0) {
        return;
    }

    changeMe = m_wspConfList->GetStringSelection();
    if(changeMe.IsEmpty()) {
        return;
    }

    DoRename(changeMe);
}
