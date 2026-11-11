#pragma once
#include "AddOptionsDialogBase.hpp"

class AddOptionsDialog : public AddOptionsDialogBase
{
public:
    AddOptionsDialog(wxWindow* parent, const wxString& value);
    ~AddOptionsDialog() override = default;
    wxString GetValue() const;
};
