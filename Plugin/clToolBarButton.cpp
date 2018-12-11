#include "clToolBarButton.h"

clToolBarButton::clToolBarButton(clToolBar* parent, wxWindowID winid, const wxBitmap& bmp, const wxString& label)
    : clToolBarButtonBase(parent, winid, bmp, label)
{
}

clToolBarButton::~clToolBarButton() {}

wxSize clToolBarButton::CalculateSize(wxDC& dc) const
{
    wxSize sz;
    sz.y = 0;
    sz.x += m_toolbar->GetXSpacer();
    if(m_bmp.IsOk()) {
        sz.x += m_bmp.GetScaledWidth();
        sz.x += m_toolbar->GetXSpacer();

        int height = m_bmp.GetScaledHeight();
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
