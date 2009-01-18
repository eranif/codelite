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
 #include "svnoptionsdlg.h"
#include "windowattrmanager.h"

SvnOptionsDlg::SvnOptionsDlg( wxWindow* parent, const SvnOptions &options, IManager *manager)
: SvnOptionsBaseDlg( parent )
, m_manager(manager)
{
	m_options = options;
	m_filePicker->SetPath(m_options.GetExePath());
	m_checkBoxUseIconsInWorkspace->SetValue(m_options.GetFlags() & SvnUseIcons ? true : false);
	m_checkBoxKeepIconsAutoUpdate->SetValue(m_options.GetFlags() & SvnKeepIconsUpdated ? true : false);
	m_checkBoxAutoAddNewFiles->SetValue(m_options.GetFlags() & SvnAutoAddFiles ? true : false);
	m_checkBoxUpdateAfterSave->SetValue(m_options.GetFlags() & SvnUpdateAfterSave ? true : false);
	m_checkBoxCaptureDiffOutput->SetValue(m_options.GetFlags() & SvnCaptureDiffOutput ? true : false);
	m_checkBoxUseExternalDiff->SetValue(m_options.GetFlags() & SvnUseExternalDiff ? true : false);
	m_diffExe->SetPath(m_options.GetDiffCmd());
	m_diffArgs->SetValue(m_options.GetDiffArgs());
	m_checkBoxKeepTagsUpToDate->SetValue(m_options.GetKeepTagUpToDate());

	m_textCtrl1->SetValue(m_options.GetPattern());
	if(m_checkBoxUseIconsInWorkspace->IsChecked() == false) {
		m_checkBoxKeepIconsAutoUpdate->Enable(false);
		m_checkBoxUpdateAfterSave->Enable(false);
	}
	GetSizer()->Fit(this);
	m_filePicker->SetFocus();

	WindowAttrManager::Load(this, wxT("SvnOptionsDialogAttr"), m_manager->GetConfigTool());
}

void SvnOptionsDlg::OnButtonOk( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_options.SetExePath(m_filePicker->GetPath());
	m_options.SetDiffCmd(m_diffExe->GetPath());
	m_options.SetDiffArgs(m_diffArgs->GetValue());
	m_options.SetPattern(m_textCtrl1->GetValue());
	m_options.SetKeepTagUpToDate(m_checkBoxKeepTagsUpToDate->IsChecked());
	SaveOptions();
	EndModal(wxID_OK);
}

void SvnOptionsDlg::OnSvnUseIcons(wxCommandEvent &e)
{
	if(e.IsChecked()){
		m_checkBoxKeepIconsAutoUpdate->Enable();
		m_checkBoxUpdateAfterSave->Enable();
	}else{
		m_checkBoxKeepIconsAutoUpdate->Enable(false);
		m_checkBoxUpdateAfterSave->Enable(false);
	}
}

void SvnOptionsDlg::SaveOptions()
{
	size_t options(0);
	if(m_checkBoxAutoAddNewFiles->IsChecked()) {
		options |= SvnAutoAddFiles;
	}

	if(m_checkBoxKeepIconsAutoUpdate->IsEnabled() && m_checkBoxKeepIconsAutoUpdate->IsChecked()) {
		options |= SvnKeepIconsUpdated;
	}

	if(m_checkBoxUpdateAfterSave->IsEnabled() && m_checkBoxUpdateAfterSave->IsChecked()) {
		options |= SvnUpdateAfterSave;
	}

	if(m_checkBoxUseIconsInWorkspace->IsChecked()) {
		options |= SvnUseIcons;
	}

	if(m_checkBoxCaptureDiffOutput->IsChecked()) {
		options |= SvnCaptureDiffOutput;
	}

	if(m_checkBoxUseExternalDiff->IsChecked()){
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
