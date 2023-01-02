#ifndef CLTOOLBARTOGGLEBUTTON_H
#define CLTOOLBARTOGGLEBUTTON_H

#include "clToolBarButton.h"

class WXDLLIMPEXP_SDK clToolBarToggleButton : public clToolBarButton
{
public:
    clToolBarToggleButton(clToolBarGeneric* parent, wxWindowID id, size_t bitmapId, const wxString& label = "");
    virtual ~clToolBarToggleButton();
};
#endif // CLTOOLBARTOGGLEBUTTON_H
