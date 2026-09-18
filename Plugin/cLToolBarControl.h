#ifndef CLTOOLBARCONTROL_H
#define CLTOOLBARCONTROL_H

#include "clToolBarButtonBase.h" // Base class: clToolBarButtonBase
#include "wxCustomControls.hpp"

class WXDLLIMPEXP_SDK clToolBarControl : public clToolBarButtonBase
{
    wxWindow* m_ctrl;

public:
    clToolBarControl(clToolBarGeneric* parent, wxWindow* control);
    ~clToolBarControl() override = default;
    wxWindow* GetControl() { return m_ctrl; }

public:
    wxSize CalculateSize(wxDC& dc) const override;
    void Render(wxDC& dc, const wxRect& rect) override;
};
#endif // CLTOOLBARCONTROL_H
