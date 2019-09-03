#include "BuildTargetDlg.h"
#include "globals.h"

BuildTargetDlg::BuildTargetDlg(wxWindow* parent, const wxString& name, const wxString& command)
    : BuildTargetDlgBase(parent)
{
    m_textCtrlaName->ChangeValue(name);
    m_textCtrlValue->ChangeValue(command);
    if(name == "build" || name == "clean") { m_textCtrlaName->Enable(false); }
    ::clSetDialogBestSizeAndPosition(this);
}

BuildTargetDlg::~BuildTargetDlg() {}

wxString BuildTargetDlg::GetTargetName() const
{
    wxString s = m_textCtrlaName->GetValue();
    s.Trim();
    return s;
}

wxString BuildTargetDlg::GetTargetCommand() const
{
    wxString s = m_textCtrlValue->GetValue();
    s.Trim();
    return s;
}
void BuildTargetDlg::OnOK_UI(wxUpdateUIEvent& event)
{
    event.Enable(!GetTargetCommand().IsEmpty() && !GetTargetName().IsEmpty());
}
