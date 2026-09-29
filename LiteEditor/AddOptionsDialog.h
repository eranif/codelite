#ifndef ADDOPTIONSDIALOG_H
#define ADDOPTIONSDIALOG_H
#include "AddOptionsDialogBase.hpp"

class AddOptionsDialog : public AddOptionsDialogBase
{
public:
    AddOptionsDialog(wxWindow* parent, const wxString& value);
    ~AddOptionsDialog() override = default;
    wxString GetValue() const;
};
#endif // ADDOPTIONSDIALOG_H
