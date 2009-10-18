//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : svnoptionsdlg.cpp
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
#include "imanager.h"
#include <wx/filedlg.h>
#include "svnoptionsdlg.h"
#include "windowattrmanager.h"
#include <wx/imaglist.h>

static wxBitmap LoadBitmapFile(const wxString &name, IManager *manager)
{
	wxBitmap bmp;
#ifdef __WXGTK__
	wxString pluginsDir(PLUGINS_DIR, wxConvUTF8);
#else
	wxString pluginsDir(manager->GetInstallDirectory() + wxT( "/plugins" ));
#endif
	wxString basePath(pluginsDir + wxT("/resources/"));

	bmp.LoadFile(basePath + name, wxBITMAP_TYPE_PNG);
	if (bmp.IsOk()) {
		return bmp;
	}
	return wxNullBitmap;
}

SvnOptionsDlg::SvnOptionsDlg( wxWindow* parent, const SvnOptions &options, IManager *manager)
		: SvnOptionsBaseDlg( parent )
		, m_manager(manager)
{
	m_options = options;
	m_textCtrlSvnExe->SetValue(m_options.GetExePath());
	m_checkBoxUseIconsInWorkspace->SetValue(m_options.GetFlags() & SvnUseIcons ? true : false);
	m_checkBoxKeepIconsAutoUpdate->SetValue(m_options.GetFlags() & SvnKeepIconsUpdated ? true : false);
	m_checkBoxAutoAddNewFiles->SetValue(m_options.GetFlags() & SvnAutoAddFiles ? true : false);
	m_checkBoxUpdateAfterSave->SetValue(m_options.GetFlags() & SvnUpdateAfterSave ? true : false);
	m_checkBoxCaptureDiffOutput->SetValue(m_options.GetFlags() & SvnCaptureDiffOutput ? true : false);
	m_checkBoxUseExternalDiff->SetValue(m_options.GetFlags() & SvnUseExternalDiff ? true : false);
	m_textCtrlDiffExe->SetValue(m_options.GetDiffCmd());
	m_diffArgs->SetValue(m_options.GetDiffArgs());
	m_checkBoxKeepTagsUpToDate->SetValue(m_options.GetKeepTagUpToDate());

	m_textCtrl1->SetValue(m_options.GetPattern());
	if (m_checkBoxUseIconsInWorkspace->IsChecked() == false) {
		m_checkBoxKeepIconsAutoUpdate->Enable(false);
		m_checkBoxUpdateAfterSave->Enable(false);
	}

	m_textCtrlSshClientCmd->SetValue(options.GetSshClient());
	m_textCtrlArguments->SetValue(options.GetSshClientArguments());

	GetSizer()->Fit(this);
	m_textCtrlSvnExe->SetFocus();

	wxImageList *il = new wxImageList(32, 32);

	il->Add(LoadBitmapFile(wxT("svn_settings.png"), m_manager));
	il->Add(LoadBitmapFile(wxT("svn_diff.png"), m_manager));
	il->Add(LoadBitmapFile(wxT("svn_ssh.png"), m_manager));

	m_listbook1->AssignImageList(il);
	m_listbook1->SetPageImage(0, 0);
	m_listbook1->SetPageImage(1, 1);
	m_listbook1->SetPageImage(2, 2);
	
	// Make the first tab of the SVN options to be selected
	m_listbook1->SetSelection(0);
	WindowAttrManager::Load(this, wxT("SvnOptionsDialogAttr"), m_manager->GetConfigTool());
}

void SvnOptionsDlg::OnButtonOk( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_options.SetExePath(m_textCtrlSvnExe->GetValue());
	m_options.SetDiffCmd(m_textCtrlDiffExe->GetValue());
	m_options.SetDiffArgs(m_diffArgs->GetValue());
	m_options.SetPattern(m_textCtrl1->GetValue());
	m_options.SetKeepTagUpToDate(m_checkBoxKeepTagsUpToDate->IsChecked());
	m_options.SetSshClient(m_textCtrlSshClientCmd->GetValue());
	m_options.SetSshClientArguments(m_textCtrlArguments->GetValue());
	SaveOptions();
	EndModal(wxID_OK);
}

void SvnOptionsDlg::OnSvnUseIcons(wxCommandEvent &e)
{
	if (e.IsChecked()) {
		m_checkBoxKeepIconsAutoUpdate->Enable();
		m_checkBoxUpdateAfterSave->Enable();
	} else {
		m_checkBoxKeepIconsAutoUpdate->Enable(false);
		m_checkBoxUpdateAfterSave->Enable(false);
	}
}

void SvnOptionsDlg::SaveOptions()
{
	size_t options(0);
	if (m_checkBoxAutoAddNewFiles->IsChecked()) {
		options |= SvnAutoAddFiles;
	}

	if (m_checkBoxKeepIconsAutoUpdate->IsEnabled() && m_checkBoxKeepIconsAutoUpdate->IsChecked()) {
		options |= SvnKeepIconsUpdated;
	}

	if (m_checkBoxUpdateAfterSave->IsEnabled() && m_checkBoxUpdateAfterSave->IsChecked()) {
		options |= SvnUpdateAfterSave;
	}

	if (m_checkBoxUseIconsInWorkspace->IsChecked()) {
		options |= SvnUseIcons;
	}

	if (m_checkBoxCaptureDiffOutput->IsChecked()) {
		options |= SvnCaptureDiffOutput;
	}

	if (m_checkBoxUseExternalDiff->IsChecked()) {
		options |= SvnUseExternalDiff;
	}

	m_options.SetFlags( options );
}

void SvnOptionsDlg::OnEnableExternalDiffViewerUI(wxUpdateUIEvent& e)
{
	bool enable = m_checkBoxUseExternalDiff->IsChecked();
	e.Enable(enable);
}

SvnOptionsDlg::~SvnOptionsDlg()
{
	WindowAttrManager::Save(this, wxT("SvnOptionsDialogAttr"), m_manager->GetConfigTool());
}

void SvnOptionsDlg::OnButtonBrowseSSHClient(wxCommandEvent& e)
{
	wxString new_path = wxFileSelector(wxT("Select a program:"), wxT(""), wxT(""), wxT(""), wxFileSelectorDefaultWildcardStr, 0, this);
	if (new_path.empty() == false) {
		m_textCtrlSshClientCmd->SetValue(new_path);
	}
}

void SvnOptionsDlg::OnBrowseSvnExe(wxCommandEvent& e)
{
	wxString path = m_textCtrlSvnExe->GetValue();
#ifdef __WXGTK__
	// to avoid warnings, we accept as default path
	// only absolute paths
	if ( !path.Trim().Trim(false).StartsWith(wxT("/")) ) {
		path.Empty();
	}
#endif
	wxString new_path = wxFileSelector(wxT("Select a program:"), path.c_str(), wxT(""), wxT(""), wxFileSelectorDefaultWildcardStr, 0, this);
	if (new_path.IsEmpty() == false) {
		m_textCtrlSvnExe->SetValue(new_path);
	}
}

void SvnOptionsDlg::OnBrowseDiffExe(wxCommandEvent& e)
{
	wxString path = m_textCtrlDiffExe->GetValue();

#ifdef __WXGTK__
	// to avoid warnings, we accept as default path
	// only absolute paths
	if ( !path.Trim().Trim(false).StartsWith(wxT("/")) ) {
		path.Empty();
	}
#endif

	wxString new_path = wxFileSelector(wxT("Select a program:"), path.c_str(), wxT(""), wxT(""), wxFileSelectorDefaultWildcardStr, 0, this);
	if (new_path.IsEmpty() == false) {
		m_textCtrlDiffExe->SetValue(new_path);
	}
}
