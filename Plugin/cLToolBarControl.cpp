#include "cLToolBarControl.h"

clToolBarControl::clToolBarControl(clToolBar* parent, wxWindow* control)
    : clToolBarButtonBase(parent, wxID_ANY, wxNullBitmap, "", kControl)
    , m_control(control)
{
}

clToolBarControl::~clToolBarControl() {}

wxSize clToolBarControl::CalculateSize(wxDC& dc) const
{
    wxUnusedVar(dc);
    return m_control->GetSize();
}

void clToolBarControl::Render(wxDC& dc, const wxRect& rect)
{
    int height = m_toolbar->GetClientRect().GetHeight();
    wxPoint pt(rect.GetPosition());
    int yy = (height - m_control->GetSize().GetHeight()) / 2 + pt.y;
    pt.y = yy;
    m_control->Move(pt);
    if(!m_control->IsShown()) { m_control->Show(); }
}
