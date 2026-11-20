#include "ChooseShellDlg.hpp"

ChooseShellDlg::ChooseShellDlg(wxWindow* parent, const wxArrayString& shells, int initialSelection)
    : ChooseShellBaseDlg(parent)
{
    m_listBoxShells->Append(shells);
    if (initialSelection != wxNOT_FOUND) {
        m_listBoxShells->SetSelection(initialSelection);
    }
    GetSizer()->Fit(this);
    CenterOnParent();
    m_listBoxShells->SetFocus();
}

ChooseShellDlg::~ChooseShellDlg() = default;

int ChooseShellDlg::GetSelection() const { return m_listBoxShells->GetSelection(); }

bool ChooseShellDlg::IsRemember() const { return m_checkBoxRemember->IsChecked(); }

void ChooseShellDlg::OnShellDoubleClicked(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_OK);
}
