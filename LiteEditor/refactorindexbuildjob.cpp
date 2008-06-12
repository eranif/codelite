//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : refactorindexbuildjob.cpp              
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
 #include <wx/progdlg.h>
#include "cppwordscanner.h"
#include <wx/filename.h>
#include "refactorindexbuildjob.h"
//#include "tokendb.h"
#include "workspace.h"

//#define POST_NEW_STATUS(msg, value, act) 
//	status = new RefactorIndexBuildJobInfo;
//	status->filename = msg;
//	status->status = value;
//	status->action = act;
//	Post(status);

RefactorIndexBuildJob::RefactorIndexBuildJob(const std::vector<wxFileName> &files)
: m_files( files )
{
}

RefactorIndexBuildJob::~RefactorIndexBuildJob()
{
}

void RefactorIndexBuildJob::Parse(const wxString &word, CppTokensMap &l)
{
//	wxProgressDialog* prgDlg = NULL;
	// Create a progress dialog
//	prgDlg = new wxProgressDialog (wxT("Gathering required information..."), wxT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), (int)m_files.size(), NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE | wxPD_CAN_ABORT);
//	prgDlg->GetSizer()->Fit(prgDlg);
//	prgDlg->Layout();
//	prgDlg->Centre();
	
//	prgDlg->Update(0, wxT("Gathering required information..."));
	size_t i=0;
	
	for ( ; i<m_files.size(); i++) {
		
		wxFileName fn = m_files.at(i);
		CppWordScanner scanner(fn.GetFullPath());

		wxString msg;
		msg << wxT("Parsing: ") << fn.GetFullName();
		// update the progress bar
//		if (!prgDlg->Update(i, msg)){
//			prgDlg->Destroy();
//			l.clear();
//			return;
//		}
//		
		scanner.Match(word, l);
	}
//	prgDlg->Destroy();
}
