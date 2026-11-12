#pragma once

#include "clToolBarButtonBase.h"

class WXDLLIMPEXP_SDK clToolBarMenuButton : public clToolBarButtonBase
{
public:
    clToolBarMenuButton(clToolBarGeneric* parent, wxWindowID winid, size_t bmpId, const wxString& label = "");
    ~clToolBarMenuButton() override = default;

public:
    wxSize CalculateSize(wxDC& dc) const override;
};
