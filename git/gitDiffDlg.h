//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __gitDiffDlg__
#define __gitDiffDlg__

#include <map>
#include "gitui.h"

class GitCommitEditor;
class IProcess;

class GitDiffDlg : public GitDiffDlgBase
{
	std::map<wxString, wxString> m_diffMap;
	wxString                     m_workingDir;
	wxString                     m_gitPath;
	
public:
	GitDiffDlg(wxWindow* parent, const wxString& workingDir);
	~GitDiffDlg();

	void SetDiff(const wxString& diff);
private:
	void OnChangeFile(wxCommandEvent& e);

	DECLARE_EVENT_TABLE();

};

#endif //__gitDiffDlg__
