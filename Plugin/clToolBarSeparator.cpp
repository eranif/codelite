#include "clToolBarSeparator.h"

clToolBarSeparator::clToolBarSeparator(clToolBar* parent)
    : clToolBarButtonBase(parent, wxID_SEPARATOR, INVALID_BITMAP_ID, "", kDisabled | kSeparator)
{
}

clToolBarSeparator::~clToolBarSeparator() {}

wxSize clToolBarSeparator::CalculateSize(wxDC& dc) const
{
    return wxSize(m_toolbar->GetXSpacer() + 1 + m_toolbar->GetXSpacer(), -1);
}

void clToolBarSeparator::Render(wxDC& dc, const wxRect& rect)
{
    // FillMenuBarBgColour(dc, rect);
    wxCoord xx = rect.GetX();
    xx += m_toolbar->GetXSpacer();

    wxColour c = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
    dc.SetPen(c.ChangeLightness(150));
    dc.DrawLine(xx, rect.GetY() + 2, xx, rect.GetY() + rect.GetHeight() - 2);
}
