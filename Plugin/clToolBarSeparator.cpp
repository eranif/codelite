#include "clToolBarSeparator.h"
#include "drawingutils.h"

clToolBarSeparator::clToolBarSeparator(clToolBar* parent)
    : clToolBarButtonBase(parent, wxID_SEPARATOR, wxNullBitmap, "", clToolBarButtonBase::kDisabled)
{
}

clToolBarSeparator::~clToolBarSeparator() {}

wxSize clToolBarSeparator::CalculateSize(wxDC& dc) const
{
    return wxSize(CL_TOOL_BAR_X_MARGIN + 1 + CL_TOOL_BAR_X_MARGIN, -1);
}

void clToolBarSeparator::Render(wxDC& dc, const wxRect& rect)
{
    wxColour penColour;
    wxColour bgColour;
    penColour = DrawingUtils::GetMenuBarBgColour();
    bgColour = penColour;

    DrawingUtils::FillMenuBarBgColour(dc, rect);
    
    wxCoord xx = rect.GetX();
    xx += CL_TOOL_BAR_X_MARGIN;

    dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    dc.DrawLine(xx, rect.GetY() + 2, xx, rect.GetY() + rect.GetHeight() - 2);
}
