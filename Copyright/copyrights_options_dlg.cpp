//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : copyrights_options_dlg.cpp              
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
 #include "copyrights_options_dlg.h"
#include <wx/filedlg.h>
#include "copyrightsconfigdata.h"
#include "iconfigtool.h"

CopyrightsOptionsDlg::CopyrightsOptionsDlg( wxWindow* parent, IConfigTool *config )
		: CopyrightsOptionsBaseDlg( parent )
		, m_conf(config)
{
	CopyrightsConfigData data;
	m_conf->ReadObject(wxT("CopyrightsConfig"), &data);
	
	m_textCtrlFileMaksing->SetValue( data.GetFileMasking());
	m_textCtrlFileName->SetValue(data.GetTemplateFilename());
	m_checkBoxBackup->SetValue( data.GetBackupFiles() );
	m_textCtrlIgnoreString->SetValue( data.GetIgnoreString() );
	m_textCtrlFileName->SetFocus();
	
	Centre();
}

void CopyrightsOptionsDlg::OnSelectFile( wxCommandEvent& event )
{
	// open file selection
	wxFileDialog *dlg = new wxFileDialog(this, _("Choose a file:"));
	if(dlg->ShowModal() == wxID_OK)
	{
		// Get the dirname
		wxString path = dlg->GetPath();
		m_textCtrlFileName->SetValue(path);
	}
	dlg->Destroy();
}

void CopyrightsOptionsDlg::OnButtonSave(wxCommandEvent& event)
{
	wxUnusedVar(event);

	CopyrightsConfigData data;
	data.SetFileMasking( m_textCtrlFileMaksing->GetValue() );
	data.SetTemplateFilename( m_textCtrlFileName->GetValue() );
	data.SetBackupFiles( m_checkBoxBackup->IsChecked() );
	data.SetIgnoreString( m_textCtrlIgnoreString->GetValue() );
	m_conf->WriteObject(wxT("CopyrightsConfig"), &data);
	EndModal(wxID_OK);
}
