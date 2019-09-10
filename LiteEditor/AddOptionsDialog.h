#ifndef ADDOPTIONSDIALOG_H
#define ADDOPTIONSDIALOG_H
#include "AddOptionsDialogBase.h"

class AddOptionsDialog : public AddOptionsDialogBase
{
public:
    AddOptionsDialog(wxWindow* parent, const wxString& value);
    virtual ~AddOptionsDialog();
    wxString GetValue() const;
};
#endif // ADDOPTIONSDIALOG_H
