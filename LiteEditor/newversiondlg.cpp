//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : newversiondlg.cpp              
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

#include "newversiondlg.h"

NewVersionDlg::NewVersionDlg( wxWindow* parent )
		: NewVersionBaseDlg( parent )
{
	m_buttonOk->SetFocus();
}

void NewVersionDlg::OnOk( wxCommandEvent& event )
{
	wxUnusedVar( event );
	EndModal(wxID_OK);
}

void NewVersionDlg::OnCancel( wxCommandEvent& event )
{
	wxUnusedVar( event );
	EndModal(wxID_CANCEL);
}

void NewVersionDlg::SetMessage(const wxString& message)
{
	m_staticText->SetLabel(message);
	GetSizer()->Fit(this);
	GetSizer()->Layout();
}

void NewVersionDlg::SetReleaseNotesURL(const wxString& url)
{
	m_hyperlink1->SetURL(url);
}
