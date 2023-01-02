#ifndef CLTOOLBARTOGGLEBUTTON_H
#define CLTOOLBARTOGGLEBUTTON_H

#include "clToolBarButton.h"
#if !wxUSE_NATIVE_TOOLBAR
class WXDLLIMPEXP_SDK clToolBarToggleButton : public clToolBarButton
{
public:
    clToolBarToggleButton(clToolBar* parent, wxWindowID id, size_t bitmapId, const wxString& label = "");
    virtual ~clToolBarToggleButton();
};
#endif // #if !wxUSE_NATIVE_TOOLBAR
#endif // CLTOOLBARTOGGLEBUTTON_H
