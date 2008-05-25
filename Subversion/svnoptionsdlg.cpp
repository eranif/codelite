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
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
 #include "svnoptionsdlg.h"

SvnOptionsDlg::SvnOptionsDlg( wxWindow* parent, const SvnOptions &options)
:
SvnOptionsBaseDlg( parent )
{
	m_options = options;
	m_filePicker->SetPath(m_options.GetExePath());
	m_checkBoxUseIconsInWorkspace->SetValue(m_options.GetFlags() & SvnUseIcons ? true : false);
	m_checkBoxKeepIconsAutoUpdate->SetValue(m_options.GetFlags() & SvnKeepIconsUpdated ? true : false);
	m_checkBoxAutoAddNewFiles->SetValue(m_options.GetFlags() & SvnAutoAddFiles ? true : false);
	m_checkBoxUpdateAfterSave->SetValue(m_options.GetFlags() & SvnUpdateAfterSave ? true : false);
	
	m_textCtrl1->SetValue(m_options.GetPattern());
	if(m_checkBoxUseIconsInWorkspace->IsChecked() == false) {
		m_checkBoxKeepIconsAutoUpdate->Enable(false);
		m_checkBoxUpdateAfterSave->Enable(false);
	}
	m_filePicker->SetFocus();
}

void SvnOptionsDlg::OnButtonOk( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_options.SetExePath(m_filePicker->GetPath());
	m_options.SetPattern(m_textCtrl1->GetValue());
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
	m_options.SetFlags( options );
}
