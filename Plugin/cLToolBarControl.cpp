#include "cLToolBarControl.h"
#include "drawingutils.h"

clToolBarControl::clToolBarControl(clToolBar* parent, wxWindow* control)
    : clToolBarButtonBase(parent, wxID_ANY, wxNullBitmap, "", kControl)
    , m_ctrl(control)
{
}

clToolBarControl::~clToolBarControl() {}

wxSize clToolBarControl::CalculateSize(wxDC& dc) const
{
    wxUnusedVar(dc);
    return m_ctrl->GetSize();
}

void clToolBarControl::Render(wxDC& dc, const wxRect& rect)
{
    wxUnusedVar(dc);
    wxRect controlRect = m_ctrl->GetRect();
    controlRect = controlRect.CenterIn(rect, wxVERTICAL);
    controlRect.SetX(rect.GetX());
    m_ctrl->Move(controlRect.GetTopLeft());
    m_ctrl->SetBackgroundColour(DrawingUtils::GetMenuBarBgColour(m_toolbar->HasFlag(clToolBar::kMiniToolBar)));
    m_ctrl->SetForegroundColour(DrawingUtils::GetTextCtrlTextColour());
    if(!m_ctrl->IsShown()) { m_ctrl->Show(); }
}
