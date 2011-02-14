#include "gitLogDlg.h"

#include "icons/icon_git.xpm"

GitLogDlg::GitLogDlg(wxWindow* parent, const wxString title)
:wxDialog(parent, wxID_ANY, title,wxDefaultPosition, wxDefaultSize,
          wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  SetIcon(wxICON(icon_git));
  m_editor = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2|wxTE_READONLY);

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  wxStdDialogButtonSizer* sizer_3 = CreateStdDialogButtonSizer(wxOK);
  wxStaticBoxSizer* sizer_1 = new wxStaticBoxSizer(wxVERTICAL,this,wxT("Log output"));
  sizer_1->Add(m_editor, 1, wxALL|wxEXPAND, 5);
  sizer->Add(sizer_1, 1, wxALL|wxEXPAND, 5);
  sizer->Add(sizer_3, 0, wxALL|wxEXPAND, 5);
  SetSizer(sizer);
  sizer->Fit(this);
  Layout();

  SetSize(1024,560);
}
void GitLogDlg::SetLog(const wxString& log)
{
  wxFont font(10, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
  wxTextAttr atr = m_editor->GetDefaultStyle();
  atr.SetFont(font);
  m_editor->SetDefaultStyle(atr);
  m_editor->AppendText(log);

}
