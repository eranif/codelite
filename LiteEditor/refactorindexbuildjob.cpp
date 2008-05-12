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
	wxProgressDialog* prgDlg = NULL;
	// Create a progress dialog
	prgDlg = new wxProgressDialog (wxT("Gathering required information..."), wxT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), (int)m_files.size(), NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE | wxPD_CAN_ABORT);
	prgDlg->GetSizer()->Fit(prgDlg);
	prgDlg->Layout();
	prgDlg->Centre();
	
	prgDlg->Update(0, wxT("Gathering required information..."));
	size_t i=0;
	
	for ( ; i<m_files.size(); i++) {
		
		wxFileName fn = m_files.at(i);
		CppWordScanner scanner(fn.GetFullPath());

		wxString msg;
		msg << wxT("Parsing: ") << fn.GetFullName();
		// update the progress bar
		if (!prgDlg->Update(i, msg)){
			prgDlg->Destroy();
			l.clear();
			return;
		}
		
		scanner.Match(word, l);
	}
	prgDlg->Destroy();
}
