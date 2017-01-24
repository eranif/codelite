//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : edit_configuration.cpp
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

#include "edit_configuration.h"
#include "manager.h"
#include "project_settings.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////

EditConfigurationDialog::EditConfigurationDialog(
    wxWindow* parent, const wxString& projectName, int id, wxString title, wxPoint pos, wxSize size, int style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_projectName(projectName)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* bSizer15;
    bSizer15 = new wxBoxSizer(wxVERTICAL);

    m_panel6 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizer17;
    bSizer17 = new wxBoxSizer(wxHORIZONTAL);

    m_configurationsList = new wxListBox(m_panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
    bSizer17->Add(m_configurationsList, 1, wxALL | wxEXPAND, 5);

    ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(m_projectName);
    if(settings) {
        ProjectSettingsCookie cookie;
        BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
        while(bldConf) {
            m_configurationsList->Append(bldConf->GetName());
            bldConf = settings->GetNextBuildConfiguration(cookie);
        }
    }
    if(m_configurationsList->GetCount() > 0) m_configurationsList->SetSelection(0);

    wxBoxSizer* bSizer18;
    bSizer18 = new wxBoxSizer(wxVERTICAL);

    m_buttonDelete = new wxButton(m_panel6, wxID_ANY, _("&Delete"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer18->Add(m_buttonDelete, 0, wxALL, 5);

    m_buttonRename = new wxButton(m_panel6, wxID_ANY, _("&Rename"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer18->Add(m_buttonRename, 0, wxALL, 5);
    bSizer17->Add(bSizer18, 0, wxEXPAND, 5);

    m_panel6->SetSizer(bSizer17);
    m_panel6->Layout();
    bSizer17->Fit(m_panel6);
    bSizer15->Add(m_panel6, 1, wxALL | wxEXPAND, 5);

    m_staticline9 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    bSizer15->Add(m_staticline9, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* bSizer16;
    bSizer16 = new wxBoxSizer(wxHORIZONTAL);

    m_buttonClose = new wxButton(this, wxID_OK, _("Close"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer16->Add(m_buttonClose, 0, wxALL, 5);

    bSizer15->Add(bSizer16, 0, wxALIGN_CENTER, 5);

    this->SetSizer(bSizer15);
    GetSizer()->Fit(this);
    Layout();

    ConnectListBoxDClick(m_configurationsList, EditConfigurationDialog::OnItemDclick);
    ConnectButton(m_buttonClose, EditConfigurationDialog::OnButtonClose);
    ConnectButton(m_buttonRename, EditConfigurationDialog::OnButtonRename);
    ConnectButton(m_buttonDelete, EditConfigurationDialog::OnButtonDelete);
}

void EditConfigurationDialog::RenameConfiguration(const wxString& oldName, const wxString& newName)
{
    ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(m_projectName);
    if(settings) {
        BuildConfigPtr bldConf = settings->GetBuildConfiguration(oldName);
        if(bldConf) {
            settings->RemoveConfiguration(oldName);
            bldConf->SetName(newName);
            settings->SetBuildConfiguration(bldConf);
            // save changes
            ManagerST::Get()->SetProjectSettings(m_projectName, settings);

            // update the control
            m_configurationsList->Clear();
            ProjectSettingsCookie cookie;
            BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
            while(bldConf) {
                m_configurationsList->Append(bldConf->GetName());
                bldConf = settings->GetNextBuildConfiguration(cookie);
            }
            if(m_configurationsList->GetCount() > 0) m_configurationsList->SetSelection(0);
        }
    }
}

void EditConfigurationDialog::OnItemDclick(wxCommandEvent& event)
{
    wxString oldName = event.GetString();
    wxTextEntryDialog* dlg = new wxTextEntryDialog(this, _("Enter New Name:"), _("Rename"), oldName);
    dlg->SetTextValidator(wxFILTER_ASCII);

    if(dlg->ShowModal() == wxID_OK) {
        wxString newName = dlg->GetValue();
        RenameConfiguration(oldName, newName);
    }
}

void EditConfigurationDialog::OnButtonClose(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_OK);
}

void EditConfigurationDialog::OnButtonRename(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString oldName = m_configurationsList->GetStringSelection();
    if(oldName.IsEmpty()) {
        return;
    }
    wxTextEntryDialog* dlg = new wxTextEntryDialog(this, _("Enter New Name:"), _("Rename"), oldName);
    dlg->SetTextValidator(wxFILTER_ASCII);

    if(dlg->ShowModal() == wxID_OK) {
        wxString newName = dlg->GetValue();
        RenameConfiguration(oldName, newName);
    }
}

void EditConfigurationDialog::OnButtonDelete(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString selection = m_configurationsList->GetStringSelection();
    if(selection.IsEmpty()) {
        return;
    }
    wxString msg(_("Remove configuration '"));
    msg << selection << _("' ?");
    if(wxMessageBox(msg, _("Confirm"), wxYES_NO | wxCANCEL | wxICON_QUESTION) == wxYES) {
        ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(m_projectName);
        if(settings) {
            settings->RemoveConfiguration(selection);
            m_configurationsList->Delete(m_configurationsList->GetSelection());
            if(m_configurationsList->GetCount() > 0) m_configurationsList->SetSelection(0);

            // save changes
            ManagerST::Get()->SetProjectSettings(m_projectName, settings);
        }
    }
}
