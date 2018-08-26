#ifndef CLTOOLBARBUTTON_H
#define CLTOOLBARBUTTON_H

#include "clToolBarButtonBase.h" // Base class: clToolBarButtonBase

class WXDLLIMPEXP_SDK clToolBarButton : public clToolBarButtonBase
{
public:
    clToolBarButton(clToolBar* parent, wxWindowID winid, const wxBitmap& bmp, const wxString& label = "");
    virtual ~clToolBarButton();

public:
    virtual wxSize CalculateSize(wxDC& dc) const;
};

#endif // CLTOOLBARBUTTON_H
