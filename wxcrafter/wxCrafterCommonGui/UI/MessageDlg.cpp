#include "MessageDlg.h"

MessageDlg::MessageDlg(wxWindow* parent, const wxString& msg, const wxString& title)
    : MessageDlgBaseClass(parent)
{
    SetMessage(msg);
    SetTitle(title);
    GetSizer()->Fit(this);
    CentreOnParent();
}

void MessageDlg::SetMessage(const wxString& msg) { m_message->SetLabel(msg); }

void MessageDlg::SetTitle(const wxString& title) { m_title->SetLabel(title); }
