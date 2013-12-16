#ifndef EDITDLG_H
#define EDITDLG_H
#include "wxcrafter_plugin.h"

class WXDLLIMPEXP_SDK EditDlg : public EditDlgBase
{
public:
    EditDlg(wxWindow* parent, const wxString &text);
    virtual ~EditDlg();
    
    wxString GetText() const {
        return m_stc10->GetText();
    }
};

WXDLLIMPEXP_SDK wxString clGetTextFromUser(const wxString &initialValue, wxWindow* parent = NULL);
#endif // EDITDLG_H
