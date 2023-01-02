#ifndef CLCUSTOMISETOOLBARDLG_H
#define CLCUSTOMISETOOLBARDLG_H

#include "ToolBarUI.h"
#include "clToolBar.h"
#include "wxCustomControls.hpp"

#include <codelite_exports.h>
#include <vector>

#if !wxUSE_NATIVE_TOOLBAR
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
#endif // #if !wxUSE_NATIVE_TOOLBAR
#endif // CLCUSTOMISETOOLBARDLG_H
