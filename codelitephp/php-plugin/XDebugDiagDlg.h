#ifndef XDEBUGDIAGDLG_H
#define XDEBUGDIAGDLG_H
#include "php_ui.h"

class XDebugDiagDlg : public XDebugDiagDlgBase
{
public:
    XDebugDiagDlg(wxWindow* parent);
    virtual ~XDebugDiagDlg();
    
    void Load(const wxString &html);
};
#endif // XDEBUGDIAGDLG_H
