//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __gitCommitListDlg__
#define __gitCommitListDlg__

#include <map>
#include "gitui.h"

class GitCommitEditor;
class IProcess;

class GitCommitListDlg : public GitCommitListDlgBase
{
	std::map<wxString, wxString> m_diffMap;
	wxString                     m_workingDir;
	wxString                     m_commandOutput;
	IProcess *                   m_process;
	wxString                     m_gitPath;
	
public:
	GitCommitListDlg(wxWindow* parent, const wxString& workingDir);
	~GitCommitListDlg();

	void SetCommitList(const wxString& commits);

private:
	void OnChangeCommit(wxListEvent& e);
	void OnChangeFile(wxCommandEvent& e);

	DECLARE_EVENT_TABLE();
	// Event handlers
	void OnProcessTerminated(wxCommandEvent &event);
	void OnProcessOutput    (wxCommandEvent &event);
};

#endif //__gitCommitListDlg__
