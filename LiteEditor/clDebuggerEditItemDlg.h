#ifndef CLDEBUGGEREDITITEMDLG_H
#define CLDEBUGGEREDITITEMDLG_H
#include "newquickwatch.h"

class clDebuggerEditItemDlg : public clDebuggerEditItemDlgBase
{
public:
    clDebuggerEditItemDlg(wxWindow* parent);
    virtual ~clDebuggerEditItemDlg();
    
    wxString GetValue() const;
};
#endif // CLDEBUGGEREDITITEMDLG_H
