#ifndef __enter_strings_dlg__
#define __enter_strings_dlg__

/**
@file
Subclass of EnterStringsDlgBase, which is generated by wxFormBuilder.
*/

#include "gui.h"

/** Implementing EnterStringsDlgBase */
class EnterStringsDlg : public EnterStringsDlgBase
{
public:
    /** Constructor */
    EnterStringsDlg(wxWindow* parent, const wxString& value);
    ~EnterStringsDlg() override = default;
    wxString GetValue() const;
    void SetMessage(const wxString& msg);
};

#endif // __enter_strings_dlg__
