#include "gitFileDiffDlg.h"
#include "gitCommitEditor.h"

#include "icons/icon_git.xpm"

GitFileDiffDlg::GitFileDiffDlg(wxWindow* parent)
:wxDialog(parent, wxID_ANY, wxT("File diff"),wxDefaultPosition, wxDefaultSize,
          wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  SetIcon(wxICON(icon_git));
   m_editor = new GitCommitEditor(this);

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  wxStdDialogButtonSizer* sizer_3 = CreateStdDialogButtonSizer(wxOK);
  wxStaticBoxSizer* sizer_1 = new wxStaticBoxSizer(wxVERTICAL,this,wxT("Diff for file"));
  sizer_1->Add(m_editor, 1, wxALL|wxEXPAND, 5);
  sizer->Add(sizer_1, 1, wxALL|wxEXPAND, 5);
  sizer->Add(sizer_3, 0, wxALL|wxEXPAND, 5);
  SetSizer(sizer);
  sizer->Fit(this);
  Layout();

}
/*******************************************************************************/
void GitFileDiffDlg::SetDiff(const wxString& diff)
{
  m_editor->SetText(diff);
  m_editor->SetReadOnly(true);
}
