#include "FloatingTextCtrl.h"
#include "SFTPBrowserDlg.h"

FloatingTextCtrl::FloatingTextCtrl(wxWindow* parent)
    : FloatingTextCtrlBase(parent)
{
    Hide();
}

FloatingTextCtrl::~FloatingTextCtrl()
{
}

bool FloatingTextCtrl::Show(bool show)
{
    if ( show ) {
        m_textCtrlInput->SetFocus();
        m_textCtrlInput->Clear();
    }
    return wxPanel::Show(show);
}

void FloatingTextCtrl::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Hide();
    GetParent()->CallAfter( &SFTPBrowserDlg::OnInlineSearchEnter );
}

void FloatingTextCtrl::OnTextUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
    GetParent()->CallAfter( &SFTPBrowserDlg::OnInlineSearch );
}

void FloatingTextCtrl::OnFocusLost(wxFocusEvent& event)
{
    event.Skip();
    Hide();
}
