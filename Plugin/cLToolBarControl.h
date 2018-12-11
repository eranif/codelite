#ifndef CLTOOLBARCONTROL_H
#define CLTOOLBARCONTROL_H

#include "clToolBarButtonBase.h" // Base class: clToolBarButtonBase

class WXDLLIMPEXP_SDK clToolBarControl : public clToolBarButtonBase
{
    wxWindow* m_ctrl;

public:
    clToolBarControl(clToolBar* parent, wxWindow* control);
    virtual ~clToolBarControl();
    wxWindow* GetControl() { return m_ctrl; }

public:
    virtual wxSize CalculateSize(wxDC& dc) const;
    void Render(wxDC& dc, const wxRect& rect);
};

#endif // CLTOOLBARCONTROL_H
