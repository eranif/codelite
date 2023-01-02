#include "clToolBarStretchableSpace.h"

#include "clSystemSettings.h"
#include "drawingutils.h"

clToolBarStretchableSpace::clToolBarStretchableSpace(clToolBarGeneric* parent)
    : clToolBarButtonBase(parent, wxID_SEPARATOR, INVALID_BITMAP_ID, "", kDisabled | kStretchalbeSpace)
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
    if(GetWidth() == 0) {
        return;
    }
    wxColour colour = clSystemSettings::GetDefaultPanelColour();
    dc.SetPen(colour);
    dc.SetBrush(colour);
    dc.DrawRectangle(rect);
}
