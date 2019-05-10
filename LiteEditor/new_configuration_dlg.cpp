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

#include "new_configuration_dlg.h"
#include "manager.h"
#include "project_settings.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////

NewConfigurationDlg::NewConfigurationDlg(wxWindow* parent, const wxString& projectName, int id, wxString title,
                                         wxPoint pos, wxSize size, int style)
    : wxDialog(parent, id, title, pos, size, style)
    , m_projectName(projectName)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);

    m_panel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizer3;
    bSizer3 = new wxBoxSizer(wxVERTICAL);

    m_staticText1 = new wxStaticText(m_panel1, wxID_ANY, _("Configuration Name:"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer3->Add(m_staticText1, 0, wxALL, 5);

    wxTextValidator validator(wxFILTER_ASCII);
    m_textConfigurationName =
        new wxTextCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, validator);
    bSizer3->Add(m_textConfigurationName, 0, wxALL | wxEXPAND, 5);
    m_textConfigurationName->SetFocus();

    m_staticText2 = new wxStaticText(m_panel1, wxID_ANY, _("Copy Settings from:"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer3->Add(m_staticText2, 0, wxALL, 5);

    wxArrayString m_choiceCopyConfigurationsChoices;
    m_choiceCopyConfigurations =
        new wxChoice(m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceCopyConfigurationsChoices, 0);

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
    bSizer3->Add(m_choiceCopyConfigurations, 0, wxALL | wxEXPAND, 5);

    m_panel1->SetSizer(bSizer3);
    m_panel1->Layout();
    bSizer3->Fit(m_panel1);
    bSizer1->Add(m_panel1, 1, wxEXPAND | wxALL, 5);

    m_staticline1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    bSizer1->Add(m_staticline1, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* bSizer2;
    bSizer2 = new wxBoxSizer(wxHORIZONTAL);

    m_buttonOK = new wxButton(this, wxID_ANY, _("&OK"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer2->Add(m_buttonOK, 0, wxALL, 5);

    m_buttonCancel = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer2->Add(m_buttonCancel, 0, wxALL, 5);

    bSizer1->Add(bSizer2, 0, wxALIGN_RIGHT, 5);

    ConnectButton(m_buttonOK, NewConfigurationDlg::OnButtonOK);
    m_buttonOK->SetDefault();
    this->SetSizer(bSizer1);
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
        wxMessageBox(_("Configuration Name is empty"), _("CodeLite"), wxOK | wxICON_INFORMATION);
        return;
    }

    ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(m_projectName);
    wxString copyFrom = m_choiceCopyConfigurations->GetStringSelection();
    BuildConfigPtr newBuildConf;

    if(copyFrom == _("-- None --")) {
        newBuildConf = new BuildConfig(NULL);
    } else {
        BuildConfigPtr origBuildConf = settings->GetBuildConfiguration(copyFrom);
        newBuildConf = origBuildConf->Clone();
    }

    newBuildConf->SetName(newConfName);
    settings->SetBuildConfiguration(newBuildConf);
    // save the settings
    ManagerST::Get()->SetProjectSettings(m_projectName, settings);
    EndModal(wxID_OK);
}
