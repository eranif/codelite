#include "clToolBarSpacer.h"

clToolBarSpacer::clToolBarSpacer(clToolBarGeneric* parent)
    : clToolBarButtonBase(parent, wxID_SEPARATOR, INVALID_BITMAP_ID, "", kDisabled | kSpacer)
{
}

wxSize clToolBarSpacer::CalculateSize(wxDC& dc) const { return wxSize(m_toolbar->GetXSpacer() * 10 + 1, -1); }

void clToolBarSpacer::Render(wxDC& dc, const wxRect& rect)
{
    wxUnusedVar(dc);
    wxUnusedVar(rect);
}
