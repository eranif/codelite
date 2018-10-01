#include "clToolBarSpacer.h"

clToolBarSpacer::clToolBarSpacer(clToolBar* parent)
    : clToolBarButtonBase(parent, wxID_SEPARATOR, wxNullBitmap, "", kDisabled | kSpacer)
{
}

clToolBarSpacer::~clToolBarSpacer() {}

wxSize clToolBarSpacer::CalculateSize(wxDC& dc) const
{
    return wxSize(m_toolbar->GetXSpacer() * 10 + 1, -1);
}

void clToolBarSpacer::Render(wxDC& dc, const wxRect& rect)
{    
    wxUnusedVar(dc);
    wxUnusedVar(rect);
}
