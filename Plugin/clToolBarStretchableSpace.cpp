#include "clToolBarStretchableSpace.h"
#include "drawingutils.h"

clToolBarStretchableSpace::clToolBarStretchableSpace(clToolBar* parent)
    : clToolBarButtonBase(parent, wxID_SEPARATOR, wxNullBitmap, "", kDisabled | kStretchalbeSpace)
{
}

clToolBarStretchableSpace::~clToolBarStretchableSpace() {}

wxSize clToolBarStretchableSpace::CalculateSize(wxDC& dc) const
{
    wxUnusedVar(dc);
    return wxSize(GetWidth(), 0);
}

void clToolBarStretchableSpace::Render(wxDC& dc, const wxRect& rect)
{
    if(GetWidth() == 0) { return; }
    wxColour colour = DrawingUtils::GetMenuBarBgColour(m_toolbar->HasFlag(clToolBar::kMiniToolBar));
    dc.SetPen(colour);
    dc.SetBrush(colour);
    dc.DrawRectangle(rect);
}
