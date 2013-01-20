#ifndef CLDEBUGGEREDITITEMDLG_H
#define CLDEBUGGEREDITITEMDLG_H

#include "newquickwatch.h"

class clDebuggerEditItemDlg : public clDebuggerEditItemDlgBase
{
public:
    clDebuggerEditItemDlg(wxWindow* parent, const wxString &initialValue);
    virtual ~clDebuggerEditItemDlg();
    
    wxString GetValue() const;
    void SetValue(const wxString &value) {
        m_textCtrl38->ChangeValue( value );
    }
};
#endif // CLDEBUGGEREDITITEMDLG_H
