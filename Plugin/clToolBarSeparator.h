#pragma once

#include "clToolBarButtonBase.h"

class WXDLLIMPEXP_SDK clToolBarSeparator : public clToolBarButtonBase
{
public:
    clToolBarSeparator(clToolBarGeneric* parent);
    ~clToolBarSeparator() override = default;

    void Render(wxDC& dc, const wxRect& rect) override;

public:
    wxSize CalculateSize(wxDC& dc) const override;
};
