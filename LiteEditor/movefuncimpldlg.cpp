//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : movefuncimpldlg.cpp              
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
 #include "wx/filename.h"
#include "movefuncimpldlg.h"

MoveFuncImplDlg::MoveFuncImplDlg( wxWindow* parent, const wxString &text, const wxString &fileName )
: MoveFuncImplBaseDlg( parent )
{
	m_preview->SetText(text);
	m_filePicker->SetPath(fileName);
	m_preview->SetFocus();
	Centre();
}

void MoveFuncImplDlg::SetText(const wxString &text)
{
	m_preview->SetText(text);
}

wxString MoveFuncImplDlg::GetText()
{
	return m_preview->GetText();
}

void MoveFuncImplDlg::SetFileName(const wxString &fileName)
{
	m_filePicker->SetPath(fileName);
}

wxString MoveFuncImplDlg::GetFileName()
{
	return m_filePicker->GetPath();
}

void MoveFuncImplDlg::OnButtonCancel(wxCommandEvent &e)
{
	wxUnusedVar(e);
	EndModal(wxID_CANCEL);
}

void MoveFuncImplDlg::OnButtonOK(wxCommandEvent &e)
{
	wxUnusedVar(e);
	//make sure that the file exist
	if(!wxFileName::FileExists(m_filePicker->GetPath())){
		wxMessageBox(wxT("File: ") + m_filePicker->GetPath() + wxT(" does not exist"), 
					 wxT("CodeLite"), wxICON_WARNING| wxOK);
		return;
	}
	EndModal(wxID_OK);
}
