#ifndef PHPDEBUGSTARTDLG_H
#define PHPDEBUGSTARTDLG_H
#include "php_ui.h"

class PHPDebugStartDlg : public PHPDebugStartDlgBase
{
public:
    PHPDebugStartDlg(wxWindow* parent);
    virtual ~PHPDebugStartDlg();
protected:
    virtual void OnDebugMethodChanged(wxCommandEvent& event);
    virtual void OnScriptToDebugUI(wxUpdateUIEvent& event);
};
#endif // PHPDEBUGSTARTDLG_H
