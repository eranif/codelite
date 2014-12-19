#ifndef __new_workspace_selection_dlg__
#define __new_workspace_selection_dlg__

#include "php_ui.h"

class NewWorkspaceSelectionDlg : public NewWorkspaceSelectionDlgBase
{
public:
    enum {
        kTypeCxx = 0,
        kTypePHP = 1,
    };

protected:

public:
    NewWorkspaceSelectionDlg(wxWindow* parent);
    virtual ~NewWorkspaceSelectionDlg();
    bool GetIsPHPWorkspace() const { return m_radioBox->GetSelection() == kTypePHP; }
};

#endif // __new_workspace_selection_dlg__
