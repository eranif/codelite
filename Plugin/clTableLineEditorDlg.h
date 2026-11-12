#pragma once

#include "wxcrafter_plugin.hpp"

#include <wx/arrstr.h>

class clTableLineEditorDlg : public clTableLineEditorBaseDlg
{
    const wxArrayString& m_columns;
    const wxArrayString& m_data;

public:
    clTableLineEditorDlg(wxWindow* parent, const wxArrayString& columns, const wxArrayString& data);
    ~clTableLineEditorDlg() override = default;

protected:
    void OnColumnSelected(wxCommandEvent& event) override;
    void DoItemSelected(int index);
};
