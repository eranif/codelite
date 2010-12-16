//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : formbuildsettingsdlg.cpp              
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
#include "formbuildsettingsdlg.h"
#include "iconfigtool.h"
#include "confformbuilder.h"
#include <wx/filedlg.h>

FormBuildSettingsDlg::FormBuildSettingsDlg( wxWindow* parent, IManager *mgr )
		: FormBuildSettingsBaseDlg( parent )
		, m_mgr(mgr)
{
	ConfFormBuilder data;
	m_mgr->GetConfigTool()->ReadObject(wxT("wxFormBuilder"), &data);
	
	m_textCtrlCommand->SetValue(data.GetCommand());
	m_textCtrlFBPath->SetValue(data.GetFbPath());
	
	m_textCtrlFBPath->SetFocus();
	GetSizer()->Fit(this);
}

void FormBuildSettingsDlg::OnButtonBrowse( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString new_path = wxFileSelector(_("Select wxFormBuilder exe:"), m_textCtrlFBPath->GetValue().c_str(), wxT(""), wxT(""), wxFileSelectorDefaultWildcardStr, 0, this);
	if (new_path.IsEmpty() == false) {
		m_textCtrlFBPath->SetValue(new_path);
	}
}

void FormBuildSettingsDlg::OnButtonOK( wxCommandEvent& event )
{
	wxUnusedVar(event);
	
	// store values
	ConfFormBuilder data;
	
	data.SetCommand(m_textCtrlCommand->GetValue());
	data.SetFbPath(m_textCtrlFBPath->GetValue());
	
	m_mgr->GetConfigTool()->WriteObject(wxT("wxFormBuilder"), &data);
	EndModal(wxID_OK);
}

void FormBuildSettingsDlg::OnButtonCancel( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}
