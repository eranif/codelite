#ifndef CLTOOLBARSPACER_H
#define CLTOOLBARSPACER_H

#include "clToolBarButtonBase.h"
#if !wxUSE_NATIVE_TOOLBAR
class WXDLLIMPEXP_SDK clToolBarSpacer : public clToolBarButtonBase
{
public:
    clToolBarSpacer(clToolBar* parent);
    virtual ~clToolBarSpacer();

    void Render(wxDC& dc, const wxRect& rect);

public:
    virtual wxSize CalculateSize(wxDC& dc) const;
};
#endif // #if !wxUSE_NATIVE_TOOLBAR
#endif // CLTOOLBARSPACER_H
