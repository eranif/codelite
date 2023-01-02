#ifndef CLTOOLBARCONTROL_H
#define CLTOOLBARCONTROL_H

#include "clToolBarButtonBase.h" // Base class: clToolBarButtonBase
#include "wxCustomControls.hpp"

class WXDLLIMPEXP_SDK clToolBarControl : public clToolBarButtonBase
{
    wxWindow* m_ctrl;

public:
    clToolBarControl(clToolBarGeneric* parent, wxWindow* control);
    virtual ~clToolBarControl();
    wxWindow* GetControl() { return m_ctrl; }

public:
    virtual wxSize CalculateSize(wxDC& dc) const;
    void Render(wxDC& dc, const wxRect& rect);
};
#endif // CLTOOLBARCONTROL_H
