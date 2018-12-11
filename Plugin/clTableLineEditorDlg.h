#ifndef CLTABLELINEEDITORDLG_H
#define CLTABLELINEEDITORDLG_H

#include "wxcrafter_plugin.h"
#include <wx/arrstr.h>

class clTableLineEditorDlg : public clTableLineEditorBaseDlg
{
    const wxArrayString& m_columns;
    const wxArrayString& m_data;

public:
    clTableLineEditorDlg(wxWindow* parent, const wxArrayString& columns, const wxArrayString& data);
    virtual ~clTableLineEditorDlg();

protected:
    virtual void OnColumnSelected(wxCommandEvent& event);
    void DoItemSelected(int index);
};
#endif // CLTABLELINEEDITORDLG_H
