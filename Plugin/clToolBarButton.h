#ifndef CLTOOLBARBUTTON_H
#define CLTOOLBARBUTTON_H

#include "clToolBarButtonBase.h" // Base class: clToolBarButtonBase
#include "wxCustomControls.hpp"

class WXDLLIMPEXP_SDK clToolBarButton : public clToolBarButtonBase
{
public:
    clToolBarButton(clToolBarGeneric* parent,
                    wxWindowID winid,
                    size_t bitmapId = INVALID_BITMAP_ID,
                    const wxString& label = "");
    ~clToolBarButton() override = default;

public:
    wxSize CalculateSize(wxDC& dc) const override;
};
#endif // CLTOOLBARBUTTON_H
