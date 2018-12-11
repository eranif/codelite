#ifndef CLTOOLBARMENUBUTTON_H
#define CLTOOLBARMENUBUTTON_H

#include "clToolBarButtonBase.h" 

class WXDLLIMPEXP_SDK clToolBarMenuButton : public clToolBarButtonBase
{
public:
    clToolBarMenuButton(clToolBar* parent, wxWindowID winid, const wxBitmap& bmp, const wxString& label = "");
    virtual ~clToolBarMenuButton();

public:
    virtual wxSize CalculateSize(wxDC& dc) const;
};

#endif // CLTOOLBARMENUBUTTON_H
