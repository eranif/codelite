//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svn_checkout_dialog.cpp
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

#include "svn_checkout_dialog.h"

#include "imanager.h"
#include "subversion2.h"
#include "windowattrmanager.h"

#include <wx/dirdlg.h>
#include <wx/msgdlg.h>

SvnCheckoutDialog::SvnCheckoutDialog(wxWindow* parent, Subversion2* plugin)
    : SvnCheckoutDialogBase(parent)
    , m_plugin(plugin)
{
    m_textCtrl20->SetValue(wxGetCwd());
    wxArrayString urls = m_plugin->GetSettings().GetUrls();
    m_comboBoxRepoURL->Append(urls);
    SetName("SvnCheckoutDialog");
    WindowAttrManager::Load(this);
}

SvnCheckoutDialog::~SvnCheckoutDialog()
{
    SvnSettingsData ssd = m_plugin->GetSettings();
    wxArrayString urls;
    wxString selection = m_comboBoxRepoURL->GetValue();

    urls = ssd.GetUrls();
    selection.Trim().Trim(false);
    if(urls.Index(selection) == wxNOT_FOUND && selection.IsEmpty() == false) {
        urls.Add(selection);
    }
    ssd.SetUrls(urls);
    m_plugin->SetSettings(ssd);
}

void SvnCheckoutDialog::OnBrowseDirectory(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString new_path = wxDirSelector(wxT(""), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if(new_path.IsEmpty() == false) {
        m_textCtrl20->SetValue(new_path);
    }
}

wxString SvnCheckoutDialog::GetTargetDir() { return m_textCtrl20->GetValue(); }

wxString SvnCheckoutDialog::GetURL() { return m_comboBoxRepoURL->GetValue(); }

void SvnCheckoutDialog::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable(m_textCtrl20->GetValue().IsEmpty() == false && m_comboBoxRepoURL->GetValue().IsEmpty() == false);
}

void SvnCheckoutDialog::OnOK(wxCommandEvent& event)
{
    // Check the target directory
    wxString targetDir = GetTargetDir();
    if(wxFileName::DirExists(targetDir)) {
        if(wxMessageBox(wxString::Format(_("The checkout directory '%s' already exists\ncontinue with the checkout?"),
                                         targetDir.c_str()),
                        _("Confirm"),
                        wxYES_NO | wxICON_WARNING) == wxNO) {
            return;
        }
    }
    event.Skip();
}
