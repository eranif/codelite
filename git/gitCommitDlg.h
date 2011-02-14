//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __gitCommitDlg__
#define __gitCommitDlg__

#include <wx/wx.h>
#include <map>
class GitCommitEditor;

class GitCommitDlg : public wxDialog
{
  GitCommitEditor* m_editor;
  wxCheckListBox* m_listBox;
  wxTextCtrl* m_commitMessage;
  std::map<wxString, wxString> m_diffMap;
  wxString m_workingDir;

  public:
    GitCommitDlg(wxWindow* parent, const wxString& repoDir);
    ~GitCommitDlg();
    
    void AppendDiff(const wxString& diff);

    wxArrayString GetSelectedFiles();
    wxString GetCommitMessage();

  private:
    void OnChangeFile(wxCommandEvent& e);
};



#endif
