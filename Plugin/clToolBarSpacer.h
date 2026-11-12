#pragma once

#include "clToolBarButtonBase.h"

class WXDLLIMPEXP_SDK clToolBarSpacer : public clToolBarButtonBase
{
public:
    clToolBarSpacer(clToolBarGeneric* parent);
    ~clToolBarSpacer() override = default;

    void Render(wxDC& dc, const wxRect& rect) override;

public:
    wxSize CalculateSize(wxDC& dc) const override;
};

