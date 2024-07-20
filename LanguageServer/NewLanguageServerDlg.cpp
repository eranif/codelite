#include "NewLanguageServerDlg.h"

#include "LSP/LanguageServerProtocol.h"
#include "LanguageServerPage.h"
#include "globals.h"
#include "macros.h"


NewLanguageServerDlg::NewLanguageServerDlg(wxWindow* parent)
    : NewLanguageServerDlgBase(parent)
{
    m_page = new LanguageServerPage(this);
    GetSizer()->Insert(0, m_page, 1, wxEXPAND);
    GetSizer()->Fit(this);
    CentreOnParent();
}

NewLanguageServerDlg::~NewLanguageServerDlg() {}

void NewLanguageServerDlg::OnOKUI(wxUpdateUIEvent& event)
{
    wxString name = m_page->GetTextCtrlName()->GetValue();
    name.Trim().Trim(false);
    event.Enable(!name.IsEmpty());
}

LanguageServerEntry NewLanguageServerDlg::GetData() const { return m_page->GetData(); }
