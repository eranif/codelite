//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svn_preferences_dialog.cpp
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

#include "svn_preferences_dialog.h"
#include <wx/imaglist.h>
#include "windowattrmanager.h"
#include "imanager.h"
#include "subversion2.h"
#include <wx/filedlg.h>
#include <wx/filename.h>

SvnPreferencesDialog::SvnPreferencesDialog(wxWindow* parent, Subversion2* plugin)
    : SvnPreferencesDialogBase(parent)
    , m_plugin(plugin)
{
    SvnSettingsData ssd;
    ssd = m_plugin->GetSettings();

    m_textCtrlDiffViewer->SetValue(ssd.GetExternalDiffViewer());
    m_textCtrlIgnorePattern->SetValue(ssd.GetIgnoreFilePattern());
    m_textCtrlSshClientArgs->SetValue(ssd.GetSshClientArgs());
    m_textCtrlSSHClient->SetValue(ssd.GetSshClient());
    m_textCtrlSvnExecutable->SetValue(ssd.GetExecutable());
    m_textCtrlMacroName->SetValue(ssd.GetRevisionMacroName());

    m_checkBoxAddToSvn->SetValue(ssd.GetFlags() & SvnAddFileToSvn);
    m_checkBoxRetag->SetValue(ssd.GetFlags() & SvnRetagWorkspace);
    m_checkBoxUseExternalDiff->SetValue(ssd.GetFlags() & SvnUseExternalDiff);
    m_checkBoxExposeRevisionMacro->SetValue(ssd.GetFlags() & SvnExposeRevisionMacro);
    m_checkBoxRenameFile->SetValue(ssd.GetFlags() & SvnRenameFileInRepo);
    m_checkBoxUsePosixLocale->SetValue(ssd.GetFlags() & SvnUsePosixLocale);

    SetName("SvnPreferencesDialog");
    WindowAttrManager::Load(this);
}

void SvnPreferencesDialog::OnBrowseSvnExe(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString path = DoGetExecutable(m_textCtrlSvnExecutable->GetValue());
    if(path.IsEmpty() == false) {
        m_textCtrlSvnExecutable->SetValue(path);
    }
}

void SvnPreferencesDialog::OnBrowseDiffViewer(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString path = DoGetExecutable(m_textCtrlDiffViewer->GetValue());
    if(path.IsEmpty() == false) {
        m_textCtrlDiffViewer->SetValue(path);
    }
}

void SvnPreferencesDialog::OnBrowseSSHClient(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString path = DoGetExecutable(m_textCtrlSSHClient->GetValue());
    if(path.IsEmpty() == false) {
        m_textCtrlSSHClient->SetValue(path);
    }
}

wxString SvnPreferencesDialog::DoGetExecutable(const wxString& basePath)
{
    wxString path;
    if(basePath.IsEmpty() == false) {
        wxFileName fn(basePath);
        if(fn.IsRelative() == false) {
            path = fn.GetPath();
        }
    }

    wxString newPath = wxFileSelector(_("Select Executable:"), path.c_str(), wxT(""), wxT(""), wxT("*"), 0, this);
    return newPath;
}

void SvnPreferencesDialog::OnButtonOK(wxCommandEvent& event)
{
    event.Skip();

    SvnSettingsData ssd;
    ssd.SetExternalDiffViewer(m_textCtrlDiffViewer->GetValue());
    ssd.SetIgnoreFilePattern(m_textCtrlIgnorePattern->GetValue());
    ssd.SetSshClient(m_textCtrlSSHClient->GetValue());
    ssd.SetSshClientArgs(m_textCtrlSshClientArgs->GetValue());
    ssd.SetExecutable(m_textCtrlSvnExecutable->GetValue());
    ssd.SetRevisionMacroName(m_textCtrlMacroName->GetValue());

    // Link Editor flag is not set via this dialog
    // so read its current value before adding the other
    // flags
    size_t flags(0);
    if(m_plugin->GetSettings().GetFlags() & SvnLinkEditor) flags = SvnLinkEditor;

    if(m_checkBoxAddToSvn->IsChecked()) {
        flags |= SvnAddFileToSvn;
    }

    if(m_checkBoxRetag->IsChecked()) {
        flags |= SvnRetagWorkspace;
    }

    if(m_checkBoxUseExternalDiff->IsChecked()) {
        flags |= SvnUseExternalDiff;
    }

    if(m_checkBoxExposeRevisionMacro->IsChecked()) {
        flags |= SvnExposeRevisionMacro;
    }

    if(m_checkBoxRenameFile->IsChecked()) {
        flags |= SvnRenameFileInRepo;
    }

    if(m_checkBoxUsePosixLocale->IsChecked()) {
        flags |= SvnUsePosixLocale;
    }

    ssd.SetFlags(flags);
    m_plugin->SetSettings(ssd);
}

void SvnPreferencesDialog::OnUseExternalDiffUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxUseExternalDiff->IsChecked());
}

void SvnPreferencesDialog::OnAddRevisionMacroUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxExposeRevisionMacro->IsChecked());
}
