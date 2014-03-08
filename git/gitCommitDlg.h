//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __gitCommitDlg__
#define __gitCommitDlg__

#include "gitui.h"
#include <map>

class GitCommitDlg : public GitCommitDlgBase
{
    std::map<wxString, wxString> m_diffMap;
    wxString m_workingDir;

public:
    GitCommitDlg(wxWindow* parent, const wxString& repoDir);
    ~GitCommitDlg();

    void AppendDiff(const wxString& diff);

    wxArrayString GetSelectedFiles();
    wxString GetCommitMessage();
    bool IsAmending() const {
        return m_checkBoxAmend->IsChecked();
    }
    
private:
    void OnChangeFile(wxCommandEvent& e);
protected:
    virtual void OnCommitOK(wxCommandEvent& event);
};

#endif
