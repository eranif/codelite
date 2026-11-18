#include "LaunchAIAgentDlg.hpp"

LaunchAIAgentDlg::LaunchAIAgentDlg(wxWindow* parent,
                                   const wxArrayString& choices,
                                   int selection,
                                   const wxString& workingDir)
    : LaunchAIAgentDlgBase(parent)
{
    m_textCtrlWorkingDirectory->SetValue(workingDir);
    m_listBox->Append(choices);
    if (!m_listBox->IsEmpty()) {
        m_listBox->Select(selection == wxNOT_FOUND ? 0 : selection);
    }

    m_listBox->CallAfter(&wxListBox::SetFocus);
    GetSizer()->Fit(this);
    CenterOnParent();
}

void LaunchAIAgentDlg::OnOkUI(wxUpdateUIEvent& event) { event.Enable(m_listBox->GetSelection() != wxNOT_FOUND); }

int LaunchAIAgentDlg::GetAgent() const { return m_listBox->GetSelection(); }
wxString LaunchAIAgentDlg::GetWorkingDirectory() const { return m_textCtrlWorkingDirectory->GetValue(); }
void LaunchAIAgentDlg::OnActivated(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_OK);
}
