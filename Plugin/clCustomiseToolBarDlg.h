#ifndef CLCUSTOMISETOOLBARDLG_H
#define CLCUSTOMISETOOLBARDLG_H

#include "ToolBarUI.h"
#include "clToolBar.h"
#include "codelite_exports.h"

#include <vector>

class WXDLLIMPEXP_SDK clCustomiseToolBarDlg : public clCustomiseToolBarBaseDlg
{
    clToolBarGeneric* m_toolbar;

public:
    clCustomiseToolBarDlg(wxWindow* parent, clToolBarGeneric* tb);
    ~clCustomiseToolBarDlg() override;

protected:
    virtual void OnOK(wxCommandEvent& event) override;
};
#endif // CLCUSTOMISETOOLBARDLG_H
