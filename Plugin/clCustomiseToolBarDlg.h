#pragma once

#include "ToolBarUI.hpp"
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
    void OnOK(wxCommandEvent& event) override;
};
