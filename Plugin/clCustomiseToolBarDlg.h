#ifndef CLCUSTOMISETOOLBARDLG_H
#define CLCUSTOMISETOOLBARDLG_H

#include "ToolBarUI.h"
#include "clToolBar.h"
#include "wxCustomControls.hpp"

#include <codelite_exports.h>
#include <vector>

class WXDLLIMPEXP_SDK clCustomiseToolBarDlg : public clCustomiseToolBarBaseDlg
{
    clToolBarGeneric* m_toolbar;
    std::vector<clToolBarButtonBase*>& m_buttons;

public:
    clCustomiseToolBarDlg(wxWindow* parent, clToolBarGeneric* tb);
    virtual ~clCustomiseToolBarDlg();

protected:
    virtual void OnOK(wxCommandEvent& event);
};
#endif // CLCUSTOMISETOOLBARDLG_H
