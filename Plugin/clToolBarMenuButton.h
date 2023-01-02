#ifndef CLTOOLBARMENUBUTTON_H
#define CLTOOLBARMENUBUTTON_H

#include "clToolBarButtonBase.h"

#if !wxUSE_NATIVE_TOOLBAR
class WXDLLIMPEXP_SDK clToolBarMenuButton : public clToolBarButtonBase
{
public:
    clToolBarMenuButton(clToolBar* parent, wxWindowID winid, size_t bmpId, const wxString& label = "");
    virtual ~clToolBarMenuButton();

public:
    virtual wxSize CalculateSize(wxDC& dc) const;
};
#endif //#if !wxUSE_NATIVE_TOOLBAR
#endif // CLTOOLBARMENUBUTTON_H
