//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : copyrights_proj_sel_dlg.cpp              
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
 #include "copyrights_proj_sel_dlg.h"
#include "workspace.h"

CopyrightsProjectSelDlg::CopyrightsProjectSelDlg( wxWindow* parent, clCxxWorkspace *wsp )
: CopyrightsProjectSelBaseDlg( parent )
{
	wxArrayString projects;
	wsp->GetProjectList( projects );
	
	for(size_t i=0; i<projects.GetCount(); i++) {
		int pos = m_checkListProjects->Append(projects.Item(i));
		m_checkListProjects->Check((unsigned int)pos, true);
	}
	m_checkListProjects->SetFocus();
}

void CopyrightsProjectSelDlg::OnCheckAll(wxCommandEvent& e)
{
	for(size_t i=0; i<m_checkListProjects->GetCount(); i++){
		m_checkListProjects->Check((unsigned int)i, true);
	}
}

void CopyrightsProjectSelDlg::OnUnCheckAll(wxCommandEvent& e)
{
	for(size_t i=0; i<m_checkListProjects->GetCount(); i++){
		m_checkListProjects->Check((unsigned int)i, false);
	}
}
void CopyrightsProjectSelDlg::GetProjects(wxArrayString& projects)
{
	for(size_t i=0; i<m_checkListProjects->GetCount(); i++){
		if(m_checkListProjects->IsChecked((unsigned int)i)) {
			projects.Add(m_checkListProjects->GetString((unsigned int)i));
		}
	}
}
