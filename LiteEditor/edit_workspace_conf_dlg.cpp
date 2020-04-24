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
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif // WX_PRECOMP

#include "edit_workspace_conf_dlg.h"
#include "macros.h"
#include "manager.h"

///////////////////////////////////////////////////////////////////////////

EditWorkspaceConfDlg::EditWorkspaceConfDlg(wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size,
                                           int style)
    : wxDialog(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* mainSizer;
    mainSizer = new wxBoxSizer(wxHORIZONTAL);

    m_wspConfList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
    mainSizer->Add(m_wspConfList, 1, wxALL | wxEXPAND, 5);

    wxBoxSizer* btnSizer;
    btnSizer = new wxBoxSizer(wxVERTICAL);

    m_buttonRename = new wxButton(this, wxID_ANY, _("&Rename"), wxDefaultPosition, wxDefaultSize, 0);
    btnSizer->Add(m_buttonRename, 0, wxALL, 5);

    m_buttonDelete = new wxButton(this, wxID_ANY, _("&Delete"), wxDefaultPosition, wxDefaultSize, 0);
    btnSizer->Add(m_buttonDelete, 0, wxALL, 5);

    m_buttonClose = new wxButton(this, wxID_CANCEL, _("&Close"), wxDefaultPosition, wxDefaultSize, 0);
    btnSizer->Add(m_buttonClose, 0, wxALL, 5);

    mainSizer->Add(btnSizer, 0, wxEXPAND, 5);

    this->SetSizer(mainSizer);
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
    std::list<WorkspaceConfigurationPtr> confs;

    confs = matrix->GetConfigurations();
    std::list<WorkspaceConfigurationPtr>::iterator iter = confs.begin();

    for(; iter != confs.end(); iter++) {
        m_wspConfList->Append((*iter)->GetName());
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
    wxString msg;
    msg << _("Remove workspace configuration '") << delMe << wxT("' ?");
    if(wxMessageBox(msg, _("CodeLite"), wxICON_QUESTION | wxYES_NO | wxCANCEL) == wxYES) {
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
