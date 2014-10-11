#include "XDebugDiagDlg.h"
#include "windowattrmanager.h"

XDebugDiagDlg::XDebugDiagDlg(wxWindow* parent)
    : XDebugDiagDlgBase(parent)
{
    WindowAttrManager::Load(this, "XDebugDiagDlg");
}

XDebugDiagDlg::~XDebugDiagDlg()
{
    WindowAttrManager::Save(this, "XDebugDiagDlg");
}

void XDebugDiagDlg::Load(const wxString& html)
{
    m_htmlWin289->SetPage(html);
}
