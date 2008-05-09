#include "cppwordscanner.h"
#include <wx/filename.h>
#include "refactorindexbuildjob.h"
#include "tokendb.h"
#include "workspace.h"

RefactorIndexBuildJob::RefactorIndexBuildJob(wxEvtHandler *parent, const std::vector<wxFileName> &files)
: Job(parent)
, m_files( files )
{
}

RefactorIndexBuildJob::~RefactorIndexBuildJob()
{
}

void RefactorIndexBuildJob::Process()
{
	TokenDb db;

	// get the path to the workspace
	wxFileName wsp_file = WorkspaceST::Get()->GetWorkspaceFileName();
	wxFileName dbfile(wsp_file.GetPath(), wsp_file.GetName() + wxT("_tokens.db"));

	db.Open(dbfile.GetFullPath());
	CppTokenList l;

//	// show some progress bar to the user
//	wxProgressDialog* prgDlg = new wxProgressDialog (wxT("Building Refactoring database ..."), wxT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), (int)files.size()*2, NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE);
//	prgDlg->GetSizer()->Fit(prgDlg);
//	prgDlg->Layout();
//	prgDlg->Centre();
//
//	prgDlg->Update(0, wxT("Parsing..."));
//	int maxVal = (int)files.size();
	size_t i=0;

	for ( ; i<m_files.size(); i++) {

		wxFileName fn = m_files.at(i);
		CppWordScanner scanner(fn.GetFullPath());

//		wxString msg;
//		msg << wxT("Parsing file: ") << fn.GetFullName();
//		prgDlg->Update((int)i, msg);
//
		// scan
		scanner.FindAll(l);
	}

	// store all tokens to the database
	db.BeginTransaction();
	size_t counter(0);

	for (size_t j=0 ; j<m_files.size(); j++) {
		wxFileName fn = m_files.at(j);
		db.DeleteByFile(fn.GetFullPath());
	}

	CppTokenList::iterator iter = l.begin();
	wxFileName curr_file;
	i = 0;
	for (; iter != l.end(); iter++, counter++) {
		if ((*iter).getFilename() != curr_file.GetFullPath()) {
			curr_file = (*iter).getFilename();
			i++;
//			wxString msg;
//			msg << wxT("Saving symbols from file: ") << curr_file.GetFullName();
//			prgDlg->Update(maxVal + i, msg);
		}

		db.Store( (*iter) );
		if (counter % 1000 == 0) {
			db.Commit();
			db.BeginTransaction();
		}
	}

//	prgDlg->Destroy();
	db.Commit();

}
