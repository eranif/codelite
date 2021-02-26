#include "clToolBarButton.h"

clToolBarButton::clToolBarButton(clToolBar* parent, wxWindowID winid, size_t bitmapId, const wxString& label)
    : clToolBarButtonBase(parent, winid, bitmapId, label)
{
}

clToolBarButton::~clToolBarButton() {}

wxSize clToolBarButton::CalculateSize(wxDC& dc) const
{
    wxSize sz;
    sz.y = 0;
    sz.x += m_toolbar->GetXSpacer();
    const wxBitmap& bmp = GetBitmap();
    if(bmp.IsOk()) {
        sz.x += bmp.GetScaledWidth();
        sz.x += m_toolbar->GetXSpacer();

        int height = bmp.GetScaledHeight();
        sz.y = wxMax(sz.GetHeight(), height);
    }
    if(!m_label.IsEmpty() && m_toolbar->IsShowLabels()) {
        wxSize textSize = dc.GetTextExtent(m_label);
        sz.x += textSize.GetWidth();
        sz.x += m_toolbar->GetXSpacer();

        int height = textSize.GetHeight();
        sz.y = wxMax(sz.GetHeight(), height);
    }
    sz.y += (2 * m_toolbar->GetYSpacer());
    return sz;
}
