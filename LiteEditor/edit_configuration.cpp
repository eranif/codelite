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

#include "edit_configuration.h"

#include "macros.h"
#include "manager.h"
#include "project_settings.h"

///////////////////////////////////////////////////////////////////////////

EditConfigurationDialog::EditConfigurationDialog(wxWindow* parent, const wxString& projectName, int id, wxString title,
                                                 wxPoint pos, wxSize size, int style)
    : EditConfigurationDialogBase(parent, id, title, pos, size, style)
    , m_projectName(projectName)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(m_projectName);
    if(settings) {
        ProjectSettingsCookie cookie;
        BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
        while(bldConf) {
            m_configurationsList->Append(bldConf->GetName());
            bldConf = settings->GetNextBuildConfiguration(cookie);
        }
    }
    if(m_configurationsList->GetCount() > 0)
        m_configurationsList->SetSelection(0);

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
            if(m_configurationsList->GetCount() > 0)
                m_configurationsList->SetSelection(0);
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
    wxString msg = wxString::Format(_("Remove configuration '%s'?"), selection);
    if(wxMessageBox(msg, _("Confirm"), wxYES_NO | wxCANCEL | wxICON_WARNING) == wxYES) {
        ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(m_projectName);
        if(settings) {
            settings->RemoveConfiguration(selection);
            m_configurationsList->Delete(m_configurationsList->GetSelection());
            if(m_configurationsList->GetCount() > 0)
                m_configurationsList->SetSelection(0);

            // save changes
            ManagerST::Get()->SetProjectSettings(m_projectName, settings);
        }
    }
}
