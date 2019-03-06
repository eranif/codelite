#include "NewLanguageServerDlg.h"
#include "LanguageServerProtocol.h"
#include <macros.h>
#include "LanguageServerPage.h"
#include "globals.h"

NewLanguageServerDlg::NewLanguageServerDlg(wxWindow* parent)
    : NewLanguageServerDlgBase(parent)
{
    m_page = new LanguageServerPage(this);
    GetSizer()->Insert(0, m_page, 1, wxEXPAND);
    ::clSetTLWindowBestSizeAndPosition(this);
}

NewLanguageServerDlg::~NewLanguageServerDlg() {}

void NewLanguageServerDlg::OnOKUI(wxUpdateUIEvent& event) { event.Enable(true); }

LanguageServerEntry NewLanguageServerDlg::GetData() const { return m_page->GetData(); }
