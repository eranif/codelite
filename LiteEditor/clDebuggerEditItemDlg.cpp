#include "clDebuggerEditItemDlg.h"

clDebuggerEditItemDlg::clDebuggerEditItemDlg(wxWindow* parent, const wxString &initialValue)
    : clDebuggerEditItemDlgBase(parent)
{
    SetValue( initialValue );
}

clDebuggerEditItemDlg::~clDebuggerEditItemDlg()
{
}

wxString clDebuggerEditItemDlg::GetValue() const
{
    return m_textCtrl38->GetValue().Trim();
}
