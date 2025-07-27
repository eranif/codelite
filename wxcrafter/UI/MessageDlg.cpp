#include "MessageDlg.h"

MessageDlg::MessageDlg(wxWindow* parent, const wxString& msg, const wxString& title)
    : MessageDlgBaseClass(parent)
{
    SetMessage(msg);
    SetTitle(title);
    GetSizer()->Fit(this);
    CentreOnParent();
}

bool MessageDlg::IsDontAnnoyChecked() const { return m_checkBoxDontAnnoy->IsChecked(); }

void MessageDlg::SetMessage(const wxString& msg) { m_message->SetLabel(msg); }

void MessageDlg::SetTitle(const wxString& title) { m_title->SetLabel(title); }
