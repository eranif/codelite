#include "clDebuggerEditItemDlg.h"

clDebuggerEditItemDlg::clDebuggerEditItemDlg(wxWindow* parent)
    : clDebuggerEditItemDlgBase(parent)
{
}

clDebuggerEditItemDlg::~clDebuggerEditItemDlg()
{
}

wxString clDebuggerEditItemDlg::GetValue() const
{
    return m_textCtrl38->GetValue().Trim();
}
