#ifndef CLGETTEXTFROMUSERDIALOG_H
#define CLGETTEXTFROMUSERDIALOG_H
#include "wxcrafter_plugin.h"

class WXDLLIMPEXP_SDK clGetTextFromUserDialog : public clGetTextFromUserBaseDialog
{
public:
    clGetTextFromUserDialog(wxWindow* parent,
                            const wxString& title,
                            const wxString& message,
                            const wxString& initialValue,
                            int charsToSelect = wxNOT_FOUND);
    virtual ~clGetTextFromUserDialog();
    
    wxString GetValue() const {
        return m_textCtrl->GetValue();
    }
};
#endif // CLGETTEXTFROMUSERDIALOG_H
