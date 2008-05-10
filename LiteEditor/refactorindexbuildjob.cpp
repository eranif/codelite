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

void RefactorIndexBuildJob::Parse(CppTokensMap &l)
{
//	TokenDb db;
	
	// get the path to the workspace
//	wxFileName wsp_file = WorkspaceST::Get()->GetWorkspaceFileName();
//	wxFileName dbfile(wsp_file.GetPath(), wsp_file.GetName() + wxT("_tokens.db"));
//
//	db.Open(dbfile.GetFullPath());

	wxProgressDialog* prgDlg = NULL;
	// Create a progress dialog
	prgDlg = new wxProgressDialog (wxT("Indexing workspace tokens..."), wxT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), (int)m_files.size(), NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE);
	prgDlg->GetSizer()->Fit(prgDlg);
	prgDlg->Layout();
	prgDlg->Centre();
	
//	if( m_full_build) {
//		// Incase of a full re-build, drop the tables and re-create the schema
//		db.RecreateSchema();
//	}
//
	prgDlg->Update(0, wxT("Building refactoring databse..."));
	size_t i=0;
	
//	RefactorIndexBuildJobInfo *status(NULL);
//	POST_NEW_STATUS(wxEmptyString, maxVal*2, Action_Reset_Gauge);
//	POST_NEW_STATUS(wxT("Building refactoring databse..."), 0, Action_Update_Gauge);
	
	for ( ; i<m_files.size(); i++) {
		
//		// test for thread cancellation
//		if(thread->TestDestroy()) {
//			return;
//		}
		
		wxFileName fn = m_files.at(i);
		CppWordScanner scanner(fn.GetFullPath());

		wxString msg;
		msg << wxT("Parsing: ") << fn.GetFullName();
		// update the progress bar
		prgDlg->Update(i, msg);
		
//		POST_NEW_STATUS(msg, i, Action_Update_Gauge);
//		// scan
		scanner.FindAll( l );
	}
	prgDlg->Destroy();
	
//	// store all tokens to the database
//	db.BeginTransaction();
//	size_t counter(0);
//
//	for (size_t j=0 ; j<m_files.size(); j++) {
//		wxFileName fn = m_files.at(j);
//		db.DeleteByFile(fn.GetFullPath());
//
//		// test for thread cancellation
//		if(thread->TestDestroy()) {
//			db.Rollback();
//			return;
//		}
//	}

//	CppTokenList::iterator iter = l.begin();
//	wxFileName curr_file;
//	i = 0;
//	for (; iter != l.end(); iter++, counter++) {
//		
////		// test for thread cancellation
////		if(thread->TestDestroy()) {
////			db.Rollback();
////			return;
////		}
//
//		if ((*iter).getFilename() != curr_file.GetFullPath()) {
//			curr_file = (*iter).getFilename();
//			i++;
//			wxString msg;
//			msg << wxT("Saving: ") << curr_file.GetFullName();
//			POST_NEW_STATUS(msg, maxVal+i, Action_Update_Gauge);
//		}
//
//		db.Store( (*iter) );
//	}
//	
//	db.Commit();
//	POST_NEW_STATUS(wxEmptyString, wxNOT_FOUND, Action_Clear_Gauge);
}
