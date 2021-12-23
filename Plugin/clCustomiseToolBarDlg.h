#ifndef CLCUSTOMISETOOLBARDLG_H
#define CLCUSTOMISETOOLBARDLG_H

#include "ToolBarUI.h"

#include <codelite_exports.h>
#include <vector>

class clToolBarButtonBase;
class clToolBar;
class WXDLLIMPEXP_SDK clCustomiseToolBarDlg : public clCustomiseToolBarBaseDlg
{
    clToolBar* m_toolbar;
    std::vector<clToolBarButtonBase*>& m_buttons;

public:
    clCustomiseToolBarDlg(wxWindow* parent, clToolBar* tb);
    virtual ~clCustomiseToolBarDlg();

protected:
    virtual void OnOK(wxCommandEvent& event);
};
#endif // CLCUSTOMISETOOLBARDLG_H
