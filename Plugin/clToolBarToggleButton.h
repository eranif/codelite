#ifndef CLTOOLBARTOGGLEBUTTON_H
#define CLTOOLBARTOGGLEBUTTON_H

#include "clToolBarButton.h"

class WXDLLIMPEXP_SDK clToolBarToggleButton : public clToolBarButton
{
public:
    clToolBarToggleButton(clToolBar* parent, wxWindowID id, const wxBitmap& bmp, const wxString& label = "");
    virtual ~clToolBarToggleButton();
};

#endif // CLTOOLBARTOGGLEBUTTON_H
