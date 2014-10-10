#include "XDebugDiagDlg.h"

XDebugDiagDlg::XDebugDiagDlg(wxWindow* parent)
    : XDebugDiagDlgBase(parent)
{
}

XDebugDiagDlg::~XDebugDiagDlg()
{
}

void XDebugDiagDlg::Load(const wxString& html)
{
    m_htmlWin289->SetPage(html);
}
