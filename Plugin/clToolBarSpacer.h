#ifndef CLTOOLBARSPACER_H
#define CLTOOLBARSPACER_H

#include "clToolBarButtonBase.h"

class WXDLLIMPEXP_SDK clToolBarSpacer : public clToolBarButtonBase
{
public:
    clToolBarSpacer(clToolBarGeneric* parent);
    virtual ~clToolBarSpacer() = default;

    void Render(wxDC& dc, const wxRect& rect);

public:
    virtual wxSize CalculateSize(wxDC& dc) const;
};

#endif // CLTOOLBARSPACER_H
