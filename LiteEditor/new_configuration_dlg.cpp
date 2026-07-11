//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : new_configuration_dlg.cpp
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

#include "new_configuration_dlg.h"

#include "manager.h"
#include "project_settings.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////

NewConfigurationDlg::NewConfigurationDlg(wxWindow* parent, const wxString& projectName, int id, wxString title,
                                         wxPoint pos, wxSize size, int style)
    : NewConfigurationDlgBase(parent, id, title, pos, size, style)
    , m_projectName(projectName)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxTextValidator validator(wxFILTER_ASCII);
    m_textConfigurationName->SetValidator(validator);
    m_textConfigurationName->SetFocus();

    // Get all configuration of the project
    m_choiceCopyConfigurations->Append(_("-- None --"));
    ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(m_projectName);
    if(settings) {
        ProjectSettingsCookie cookie;
        BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
        while(bldConf) {
            m_choiceCopyConfigurations->Append(bldConf->GetName());
            bldConf = settings->GetNextBuildConfiguration(cookie);
        }
    }
    m_choiceCopyConfigurations->SetSelection(0);

    m_panel1->Layout();
    // bSizer3->Fit(m_panel1);

    ConnectButton(m_buttonOK, NewConfigurationDlg::OnButtonOK);
    m_buttonOK->SetDefault();

    CentreOnScreen();
    GetSizer()->Fit(this);
    CenterOnParent();
    this->Layout();
}

void NewConfigurationDlg::OnButtonOK(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString newConfName;
    newConfName = m_textConfigurationName->GetValue();
    newConfName = newConfName.Trim();
    newConfName = newConfName.Trim(false);
    if(newConfName.IsEmpty()) {
        wxMessageBox(_("Configuration Name is empty"), wxT("CodeLite"), wxOK | wxICON_INFORMATION);
        return;
    }

    ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(m_projectName);
    wxString copyFrom = m_choiceCopyConfigurations->GetStringSelection();
    BuildConfigPtr newBuildConf;

    if(copyFrom == _("-- None --")) {
        newBuildConf = std::make_shared<BuildConfig>(nullptr);
    } else {
        BuildConfigPtr origBuildConf = settings->GetBuildConfiguration(copyFrom);
        newBuildConf.reset(origBuildConf->Clone());
    }

    newBuildConf->SetName(newConfName);
    settings->SetBuildConfiguration(newBuildConf);
    // save the settings
    ManagerST::Get()->SetProjectSettings(m_projectName, settings);
    EndModal(wxID_OK);
}
